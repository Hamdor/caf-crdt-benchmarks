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

struct config : public crdt_config {
  config() : crdt_config() {
    add_crdt<gset<rep>>("tp");
  }
};

class master : public event_based_actor {
public:
  master(actor_config& cfg, uint32_t ticks)
      : event_based_actor(cfg), current_ticks_{0}, max_ticks_{ticks},
        times_{this, "tp://bench"} {
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
  listener(actor_config& cfg, actor master)
      : event_based_actor(cfg), items_{this, "tp://bench"} {
    monitor(master);
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

int main(int argc, char** argv) {
  if (argc != 3)
    return std::cout << "Usage: ./crdt_bench ticks listeners"
                     << std::endl, -1;
  auto ticks = static_cast<uint32_t>(std::stoi(argv[1]));
  auto listeners = static_cast<uint32_t>(std::stoi(argv[2]));
  config cfg{};
  actor_system system{cfg};
  auto m = system.spawn<master>(ticks);
  for (uint32_t i = 0; i < listeners; ++i)
    system.spawn<listener>(m);
  anon_send(m, tick_atom::value);
}
