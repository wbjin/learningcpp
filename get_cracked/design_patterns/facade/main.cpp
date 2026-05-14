#include <iostream>

/*
 * Assume we have some kind of external SDK or complex subsystem for allocating
 * cloud resources. The different controllers must be used together correctly
 * and the correct API calls must be made.
 * */
class ComputeController {
public:
  void Allocate() { std::cout << "ComputeController::Allocate()\n"; }

  // More complex operations below...
};

class StorageController {
public:
  void Allocate() { std::cout << "StorageController::Allocate()\n"; }

  // AttachToNode must be called after a compute instance is allocated
  void AttachToNode() { std::cout << "StorageController::AttachToNode()\n"; }

  // More complex operations below...
};

class NetworkController {
public:
  void Configure() { std::cout << "NetworkController::Configure()\n"; }

  void CreateSubnet() { std::cout << "NetworkController::CreateSubnet()\n"; }

  // More complex operations below...
};

/*
 * A CloudController facade interfac simplifies the process of creating a new
 * node in a cloud proivder.
 * */
class CloudController {
public:
  void CreateNode() {
    std::cout << "CloudController::CreateNode()\n";
    compute_ctl_.Allocate();
    network_ctl_.CreateSubnet();
    storage_ctl_.Allocate();
    storage_ctl_.AttachToNode();
    network_ctl_.Configure();
  }

private:
  ComputeController compute_ctl_{};
  StorageController storage_ctl_{};
  NetworkController network_ctl_{};
};

int main() {
  CloudController controller {};
  controller.CreateNode();
}
