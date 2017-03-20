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
    add_crdt<gset<std::string>>("bench");
  }
};

///
class master : public event_based_actor {
public:
  master(actor_config& cfg, uint32_t num_workers, uint32_t num_per_worker)
    : event_based_actor(cfg), num_workers_{num_workers}, num_per_worker_{num_per_worker},
      items_{this, "bench"} {
    // nop
  }

protected:
  behavior make_behavior() override {
    return {
      [&](sub_atom, const actor& sub) {
        workers_.emplace(sub);
        if (workers_.size() == num_workers_) {
          for (auto& w : workers_)
            send(w, go_atom::value);
          //begin_ = std::chrono::high_resolution_clock::now();
        }
      },
      [&](notify_atom, const gset<std::string>& delta) {
        items_.merge(delta);
        if (items_.size() == num_per_worker_ * num_workers_) {
          //end_ = std::chrono::high_resolution_clock::now();
	  //auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - begin_);
	  //std::cout << diff.count() << std::endl;
          quit();
	}
      }
    };
  }
private:
  //std::chrono::high_resolution_clock::time_point begin_;
  //std::chrono::high_resolution_clock::time_point end_;
  uint32_t num_workers_;
  uint32_t num_per_worker_;
  std::set<actor> workers_;
  gset<std::string> items_;
};

///
class generator : public event_based_actor {
public:
  generator(actor_config& cfg, actor master, uint32_t to_generate, const std::string& id)
    : event_based_actor(cfg), generated_{0}, to_generate_{to_generate},
      id_{id}, items_{this, "bench"} {
    send(master, sub_atom::value, this);
  }

protected:
  virtual behavior make_behavior() override {
    return {
      [&](go_atom& atm) {
	if (generated_ >= to_generate_) {
	  quit();
	  return;
	}
	std::stringstream ss;
	ss << id_ << generated_++;
	items_.insert(ss.str());
	send(this, atm);
      },
      [&](notify_atom, const gset<std::string>& delta) {
        items_.merge(delta);
      }
    };
  }

private:
  uint32_t generated_;
  uint32_t to_generate_;
  std::string id_;
  gset<std::string> items_;
};

int main(int argc, char** argv) {
  if (argc != 3)
    std::cout << "Usage: ./crdt_bench num_workers num_entires_per_worker"
	      << std::endl;
  auto num_workers = static_cast<uint32_t>(std::stoi(argv[1]));
  auto num_entries = static_cast<uint32_t>(std::stoi(argv[2]));
  config cfg;
  actor_system system{cfg};
  auto master_actor = system.spawn<master>(num_workers, num_entries);
  for (uint32_t i = 0; i < num_workers; ++i) {
    std::stringstream ss;
    ss << "w-" << i << "-";
    system.spawn<generator>(master_actor, num_entries, ss.str());
  }
}
