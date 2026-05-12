#include <iostream>
#include <memory>

/*
 * Different types of storage controllers may be created per cloud provider
 * */
class StorageController {
public:
  virtual void SetSize(size_t size) const = 0;
  virtual size_t GetSize() = 0;

  virtual ~StorageController() = default;
};

class AWSStorageController : public StorageController {
public:
  void SetSize(size_t size) const override {
    std::cout << "[AWS StorageController] SetSize(" << size << ")\n";
  }

  size_t GetSize() override {
    std::cout << "[AWS StorageController] GetSize()\n";
    return 1000;
  }
};

class GCPStorageController : public StorageController {
public:
  void SetSize(size_t size) const override {
    std::cout << "[GCP StorageController] SetSize(" << size << ")\n";
  }

  size_t GetSize() override {
    std::cout << "[GCP StorageController] GetSize()\n";
    return 2000;
  }
};

/*
 * Different types of compute controllers may be created per cloud provider
 * */
class ComputeController {
public:
  virtual void AllocateInstance() = 0;
  virtual void DeallocateInstance(std::string_view uuid) = 0;

  virtual ~ComputeController() = default;
};

class AWSComputeController : public ComputeController {
public:
  void AllocateInstance() override {
    std::cout << "[AWS ComputeController] AllocateInstance()\n";
  }

  void DeallocateInstance(std::string_view uuid) override {
    std::cout << "[AWS ComputeController] DeallocateInstance(" << uuid << ")\n";
  }
};

class GCPComputeController : public ComputeController {
public:
  void AllocateInstance() override {
    std::cout << "[GCP ComputeController] AllocateInstance()\n";
  }

  void DeallocateInstance(std::string_view uuid) override {
    std::cout << "[GCP ComputeController] DeallocateInstance(" << uuid << ")\n";
  }
};

/*
 * Define a factory base type that defines the interface for creating a family
 * of subtypes.
 * */
class ControllerFactory {
public:
  virtual std::unique_ptr<StorageController> CreateStorageController() = 0;
  virtual std::unique_ptr<ComputeController> CreateComputeController() = 0;

  virtual ~ControllerFactory() = default;
};

/*
 * Interface for different families of subtypes inherit off of the common
 * factory interface
 * */
class AWSControllerFactory : public ControllerFactory {
public:
  std::unique_ptr<StorageController> CreateStorageController() override {
    return std::make_unique<AWSStorageController>();
  }

  std::unique_ptr<ComputeController> CreateComputeController() override {
    return std::make_unique<AWSComputeController>();
  }
};

class GCPControllerFactory : public ControllerFactory {
public:
  std::unique_ptr<StorageController> CreateStorageController() override {
    return std::make_unique<GCPStorageController>();
  }

  std::unique_ptr<ComputeController> CreateComputeController() override {
    return std::make_unique<GCPComputeController>();
  }
};

int main() {
  constexpr const std::string_view CLOUD_PROVIDER = "GCP";
  std::unique_ptr<ControllerFactory> factory{};
  std::unique_ptr<StorageController> storage_controller{};
  std::unique_ptr<ComputeController> compute_controller{};

  if (CLOUD_PROVIDER == "GCP") {
    factory = std::make_unique<GCPControllerFactory>();
  } else if (CLOUD_PROVIDER == "AWS") {
    factory = std::make_unique<AWSControllerFactory>();
  } else {
    throw std::runtime_error("Unknown cloud provider");
  }

  storage_controller = factory->CreateStorageController();
  compute_controller = factory->CreateComputeController();

  storage_controller->GetSize();
  storage_controller->SetSize(1234);

  compute_controller->AllocateInstance();
  compute_controller->DeallocateInstance("abc2");
}
