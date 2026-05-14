#include <iostream>
#include <memory>
#include <string>

/*
 * Base class that defines the interface for various implementations
 * */
class Exporter {
public:
  virtual ~Exporter() = default;
  virtual void Export(const std::string &payload) = 0;
};

/*
 * There is usually a "base" level (the innermost layer) when using the
 * decorator pattern
 * */
class NullExporter : public Exporter {
public:
  void Export(const std::string &payload) override {
    std::cout << "NullExporter::Export()\n";
  }
};

/*
 * Each decorator implements the same interface as the base class. They also
 * have a reference to call the next layer and does so after it's operations
 * are complete.
 * */
class DiskExporterDecorator : public Exporter {
public:
  explicit DiskExporterDecorator(std::unique_ptr<Exporter> exporter)
      : exporter_{std::move(exporter)} {}

  void Export(const std::string &payload) override {
    std::cout << "DiskExporterDecorator::Export()\n";
    exporter_->Export(payload);
  }

private:
  std::unique_ptr<Exporter> exporter_ {};
};

class NetworkExporterDecorator : public Exporter {
public:
  explicit NetworkExporterDecorator(std::unique_ptr<Exporter> exporter)
      : exporter_{std::move(exporter)} {};

  void Export(const std::string &payload) override { 
    std::cout << "NetworkExporterDecorator::Export()\n";
    exporter_->Export(payload);
  }

private:
  std::unique_ptr<Exporter> exporter_ {};
};

class FilterDecorator : public Exporter {
public:
  explicit FilterDecorator(std::unique_ptr<Exporter> exporter)
      : exporter_{std::move(exporter)} {};

  void Export(const std::string &payload) override { 
    std::cout << "FilterDecorator::Export()\n";
    if (payload.size() > 10000)
      return;
    exporter_->Export(payload);
  }

private:
  std::unique_ptr<Exporter> exporter_ {};
};

int main() {
  std::unique_ptr<Exporter> exporter = std::make_unique<NullExporter>();
  exporter->Export("payload1");

  exporter = std::make_unique<DiskExporterDecorator>(std::move(exporter));
  exporter->Export("payload2");

  exporter = std::make_unique<NetworkExporterDecorator>(std::move(exporter));
  exporter->Export("payload3");

  exporter = std::make_unique<FilterDecorator>(std::move(exporter));
  exporter->Export("payload4");
}
