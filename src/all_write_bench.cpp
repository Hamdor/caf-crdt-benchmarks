#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <caf/crdt/all.hpp>

using namespace caf;
using namespace caf::crdt;
using namespace caf::crdt::types;

using namespace std::chrono;

using tick_atom = atom_constant<atom("tick")>;
using quit_atom = atom_constant<atom("quit")>;

struct port_dummy : public event_based_actor {
  using event_based_actor::event_based_actor;
};

struct config : public crdt_config {
  config() : crdt_config() {
    load<io::middleman>();
    add_crdt<gcounter<int>>("gcounter<int>");
    set_refresh_ids_interval(std::chrono::seconds(20));
    set_state_interval(std::chrono::minutes(5));
  }
};

class master : public event_based_actor {
public:
  master(actor_config& cfg, uint32_t ticks, uint32_t all)
      : event_based_actor(cfg), current_ticks_{0}, my_max_ticks_{ticks}, max_ticks_{all},
        items_{this, "gcounter<int>://bench"} {
    // nop
  }

protected:
  behavior make_behavior() override {
    return {
      [&](tick_atom tick) {
        current_ticks_++;
        items_.increment();
        if (current_ticks_ == my_max_ticks_) {
          if (items_.count() == max_ticks_)
            delayed_send(this, seconds(10), quit_atom::value);
          return;
        }
        delayed_send(this, milliseconds(10), tick_atom::value);
      },
      [&](notify_atom, const gcounter<int>& delta) {
        items_.merge(delta);
        if (items_.count() == max_ticks_) {
          delayed_send(this, seconds(10), quit_atom::value);
          return;
        }
      }
    };
  }

private:
  uint32_t current_ticks_;
  uint32_t my_max_ticks_;
  uint32_t max_ticks_;
  gcounter<int> items_;
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
  actor m = system.spawn<master>(ticks, ticks * num_nodes);
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
  std::this_thread::sleep_for(std::chrono::seconds(10));
  anon_send(m, tick_atom::value);
}
