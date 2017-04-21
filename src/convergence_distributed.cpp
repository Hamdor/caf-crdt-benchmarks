#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <caf/crdt/all.hpp>

using namespace caf;
using namespace caf::crdt;
using namespace caf::crdt::types;

using namespace std::chrono;

using clk = high_resolution_clock;
using tp  = clk::time_point;
using rep = clk::duration::rep;

using tick_atom = atom_constant<atom("tick")>;

struct port_dummy : public event_based_actor {
  using event_based_actor::event_based_actor;
};

struct config : public crdt_config {
  config() : crdt_config() {
    load<io::middleman>();
    add_crdt<gset<rep>>("gset<rep>");
  }
};

class master : public event_based_actor {
public:
  master(actor_config& cfg, uint32_t ticks)
      : event_based_actor(cfg), current_ticks_{0}, max_ticks_{ticks},
        times_{this, "gset<rep>://bench"} {
    // nop
  }

protected:
  behavior make_behavior() override {
    return {
      [&](tick_atom& tick) {
        if (++current_ticks_ > max_ticks_)
          quit();
        times_.insert(duration_cast<std::chrono::microseconds>(
                      clk::now().time_since_epoch()).count());
        delayed_send(this, milliseconds(250), tick);
      }
    };
  }

private:
  uint32_t current_ticks_;
  uint32_t max_ticks_;
  gset<rep> times_;
};

class listener : public event_based_actor {
public:
  listener(actor_config& cfg, optional<actor> master)
      : event_based_actor(cfg), items_{this, "gset<rep>://bench"} {
    if (master)
      monitor(*master);
  }

protected:
  behavior make_behavior() override {
    set_down_handler([&](down_msg&) { quit(); });
    return {
      [&](notify_atom, const gset<rep>& delta) {
        auto now = clk::now().time_since_epoch();
        auto dur = duration_cast<microseconds>(now).count();
        auto obj = *delta.cbegin();
        aout(this) << dur - obj << "\n";
        items_.merge(delta);
      }
    };
  }

private:
  gset<rep> items_;
};

int print_help() {
  return std::cout << "Usage: ./crdt_bench ticks listeners num_nodes"
                   << std::endl, -1;
}

int main(int argc, char** argv) {
  if (argc != 4)
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
  system.middleman().publish(system.spawn<port_dummy>(), 1234);
  // Sleep some time, all instances need to open the port
  std::this_thread::sleep_for(std::chrono::seconds(2));
  for (auto& ip : ips) {
    scoped_actor self{system};
    self->send(system.middleman().actor_handle(), connect_atom::value,
               ip, uint16_t{1234});
  }
  // --- Start actors
  optional<actor> m;
  if (ticks != 0)
    m = system.spawn<master>(ticks);
  for (uint32_t i = 0; i < listeners; ++i)
    system.spawn<listener>(m);
  if (m)
    anon_send(*m, tick_atom::value);
}
