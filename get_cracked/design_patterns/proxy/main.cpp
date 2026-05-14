#include <memory>
#include <cstdio>

/*
 * A base implementation for a concrete object and a proxy object
 * */
class RenderEngine {
public:
  virtual ~RenderEngine() = default;
  virtual void Render() = 0;
};

class DirectXRenderEngine : public RenderEngine {
public:
  DirectXRenderEngine() {
    std::printf("DirectXRenderEngine()\n");
    // Assume some kind of heavy initialization
  }

  void Render() override {
    std::printf("DirectXRenderEngine::Render()\n");
  }
};

// Proxy that does lazy loading
class DirectXRenderEngineProxy : public RenderEngine {
public:
  void Render() override {
    if (!engine_) {
      engine_ = std::make_unique<DirectXRenderEngine>();
    }

    engine_->Render();
  }

private:
  std::unique_ptr<DirectXRenderEngine> engine_ {};
};

int main() {
  std::unique_ptr<RenderEngine> render = std::make_unique<DirectXRenderEngineProxy>();
  render->Render();
}
