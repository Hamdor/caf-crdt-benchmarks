#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <caf/crdt/all.hpp>

using namespace caf;
using namespace caf::crdt;
using namespace caf::crdt::types;

using sub_atom = atom_constant<atom("sub")>;
using go_atom  = atom_constant<atom("go")>;

struct config : public crdt_config {
  config() : crdt_config() {
    add_crdt<gset<int>>("bench");
  }
};

///
class generator : public event_based_actor {
public:
  generator(actor_config& cfg)
    : event_based_actor(cfg), val_{0},
      items_{this, "bench"} {
    // nop
  }

protected:
  virtual behavior make_behavior() override {
    send(this, go_atom::value);
    return {
      [&](go_atom& atm) {
	if (items_.size() >= 10000) {
	  quit();
	  return;
	}
	items_.insert(val_++);
	send(this, atm);
      },
      [&](notify_atom, const gset<int>&) {
        // nop
      }
    };
  }

private:
  int val_ = 0;
  gset<int> items_;
};

struct state {
  state(event_based_actor* self) : crdt{self, "bench"} {}
  gset<int> crdt;
};

void sub_actor(stateful_actor<state>* self) {
  self->become(
    [=](notify_atom, const gset<int>& other) {
      self->state.crdt.merge(other);
      if (self->state.crdt.size() >= 10000)
        self->quit();
    }
  );
}


int main(int argc, char** argv) {
  if (argc != 3)
    std::cout << "Usage: ./crdt_bench num_workers num_entires_per_worker"
	      << std::endl;
  auto num_workers = static_cast<uint32_t>(std::stoi(argv[1]));
  auto num_entries = static_cast<uint32_t>(std::stoi(argv[2]));
  config cfg;
  actor_system system{cfg};
  for (int i = 0; i < num_workers; ++i)
    system.spawn(sub_actor);
  system.spawn<generator>();
}
