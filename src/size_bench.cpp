#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <caf/crdt/all.hpp>

using namespace caf;
using namespace caf::crdt;
using namespace caf::crdt::types;

using namespace std::chrono;

using tick_atom = atom_constant<atom("tick")>;
using master_atom = atom_constant<atom("master")>;
using register_atom = atom_constant<atom("reg")>;
using set_master_atom = atom_constant<atom("smaster")>;

struct port_dummy : public event_based_actor {
  using event_based_actor::event_based_actor;
};

struct config : public crdt_config {
  config() : crdt_config() {
    load<io::middleman>();
    add_crdt<gset<int>>("gset<int>");
    set_refresh_ids_interval(std::chrono::seconds(20));
    set_state_interval(std::chrono::minutes(5));
  }
};

class master : public event_based_actor {
public:
  master(actor_config& cfg, uint32_t ticks, uint32_t assumed_reg)
      : event_based_actor(cfg), assumed_reg_{assumed_reg}, registered_{0}, current_ticks_{0}, max_ticks_{ticks},
        times_{this, "gset<int>://bench"} {
    // nop
  }

protected:
  behavior make_behavior() override {
    set_down_handler([&](scheduled_actor*, down_msg&) {
      registered_--;
      if (registered_ == 0)
        quit();
    });
    return {
      [&](register_atom) {
	monitor(actor_cast<actor>(current_sender()));
        registered_++;
	if (registered_ == assumed_reg_)
	  send(this, tick_atom::value);
      },
      [&](tick_atom& tick) {
        if (times_.size() == max_ticks_)
	  return;
	times_.insert(current_ticks_++);
        delayed_send(this, milliseconds(10), tick);
      },
      [&](notify_atom, const gset<int>&) {
        // nop
      }
    };
  }

private:
  uint32_t assumed_reg_;
  uint32_t registered_;
  uint32_t current_ticks_;
  uint32_t max_ticks_;
  gset<int> times_;
};

class listener : public event_based_actor {
public:
  listener(actor_config& cfg, uint32_t size)
      : event_based_actor(cfg), items_{this, "gset<int>://bench"}, assumed_size{size} {
    // nop
  }

protected:
  behavior make_behavior() override {
    return {
      [&](set_master_atom, actor master) {
        send(master, register_atom::value);
      },
      [&](notify_atom, const gset<int>& delta) {
        items_.merge(delta);
	if (items_.size() >= assumed_size)
          quit();
      }
    };
  }
private:
  gset<int> items_;
  uint32_t assumed_size;
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
  if (argc != 4 + num_nodes)
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
  actor l;
  // spawn master early if needed
  if (listeners == 0) {
    m = system.spawn<master>(ticks, num_nodes - 1);
    system.registry().put(master_atom::value,
		          actor_cast<strong_actor_ptr>(m));
  } else
    l = system.spawn<listener>(ticks);
  // Sleep some time, all instances need to open the port
  std::this_thread::sleep_for(std::chrono::seconds(20));
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
  if (l)
    anon_send(l, set_master_atom::value, m);
}
