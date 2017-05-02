#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <caf/crdt/all.hpp>

using namespace caf;
using namespace caf::crdt;
using namespace caf::crdt::types;

using namespace std::chrono;

using tick_atom = atom_constant<atom("tick")>;
using master_atom = atom_constant<atom("master")>;

struct port_dummy : public event_based_actor {
  using event_based_actor::event_based_actor;
};

struct config : public crdt_config {
  config() : crdt_config() {
    load<io::middleman>();
    add_crdt<gset<int>>("gset<int>");
  }
};

class master : public event_based_actor {
public:
  master(actor_config& cfg, uint32_t ticks)
      : event_based_actor(cfg), current_ticks_{0}, max_ticks_{ticks},
        times_{this, "gset<int>://bench"} {
    // nop
  }

protected:
  behavior make_behavior() override {
    return {
      [&](tick_atom& tick) {
        if (times_.size() > max_ticks_) quit();
        times_.insert(current_ticks_++);
        delayed_send(this, milliseconds(10), tick);
      },
      [&](notify_atom, const gset<int>&) {
        // nop
      }
    };
  }

private:
  uint32_t current_ticks_;
  uint32_t max_ticks_;
  gset<int> times_;
};

class listener : public event_based_actor {
public:
  listener(actor_config& cfg, actor master)
      : event_based_actor(cfg), items_{this, "gset<int>://bench"} {
    monitor(master);
  }

protected:
  behavior make_behavior() override {
    set_down_handler([&](down_msg&) { quit(); });
    return {
      [&](notify_atom, const gset<int>& delta) {
        items_.merge(delta);
      }
    };
  }

private:
  gset<int> items_;
};

int print_help() {
  return std::cout << "Usage: ./crdt_bench ticks listeners num_nodes ips..."
                   << std::endl, -1;
}

int main(int argc, char** argv) {
  if (argc < 4)
    return print_help();
  auto ticks = static_cast<uint32_t>(std::stoi(argv[1]));
  auto listeners = static_cast<uint32_t>(std::stoi(argv[2]));
  auto num_nodes = static_cast<uint32_t>(std::stoi(argv[3]));
  std::string host_name = argv[num_nodes + 4];
  if (argc != 5 + num_nodes)
    return print_help();
  std::vector<std::string> ips;
  for (int i = 4; i < argc; ++i)
    ips.emplace_back(argv[i]);
  config cfg{};
  actor_system system{cfg};
  // Open local port with dummy
  auto& mm = system.middleman();
  mm.publish(system.spawn<port_dummy>(), 1234);
  actor m;
  // spawn master early if needed
  if (ticks != 0) {
    m = system.spawn<master>(ticks);
    system.registry().put(master_atom::value,
		          actor_cast<strong_actor_ptr>(m));
  }
  // Sleep some time, all instances need to open the port
  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::set<node_id> nids;
  for (auto& ip : ips) {
    scoped_actor self{system};
    auto r = self->request(system.middleman().actor_handle(), seconds(20), connect_atom::value,
                           ip, uint16_t{1234});
    r.receive([&](node_id& n, strong_actor_ptr&, std::set<std::string>&) { nids.emplace(n); },
	      [&](error&) { /* ignore ... */ });
  }
  // --- find master
  if (!m) {
    for (auto& nid : nids) {
      auto query = mm.remote_lookup(master_atom::value, nid);
      if (query) {
        m = actor_cast<actor>(query);
        break;
      }
    }
  }
  // start listeners
  for (uint32_t i = 0; i < listeners; ++i)
    system.spawn<listener>(m);
  // start algorithm
  if (ticks != 0)
    anon_send(m, tick_atom::value);
}
