#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <format>

/*
 * Extrinsic state of a Particle object
 * */
struct ParticleType {
  std::string color {};
  std::string sprite {};
};

/*
 * Intrinsic state of a particle object
 * */
struct Particle {
  float x {}, y {};
  float dx {}, dy {};
  float mass {};

  std::shared_ptr<const ParticleType> type {};
};

class ParticleTypeFactory {
public:
  std::shared_ptr<const ParticleType> GetType(std::string color, std::string sprite) {
    std::string key = color + "," + sprite;
    if (types_.contains(key)) {
      return types_.at(key);
    }

    std::cout << std::format(
        "ParticleTypeFactory::GetType(color={}, sprite={}). Creating "
        "new ParticleType object\n",
        color, sprite);
    types_.insert({key, std::make_shared<const ParticleType>(
                            ParticleType{color, sprite})});
    return types_.at(key);
  }

private:
  std::unordered_map<std::string, std::shared_ptr<const ParticleType>> types_ {};
};

class ParticleFactory {
public:
  Particle GetSmoke(float x, float y, float dx, float dy, float mass) {
    std::printf(
        "ParticleFactory::GetSmoke(x=%f, y=%f, dx=%f, dy=%f, mass=%f)\n", x, y,
        dx, dy, mass);
    return Particle {
      .x = x,
      .y = y,
      .dx = dx,
      .dy = dy,
      .mass = mass,
      .type = type_factory_.GetType("grey", "smoke.png")
    };
  }

private:
  ParticleTypeFactory type_factory_ {};
};

int main() {
  ParticleFactory factory {};
  factory.GetSmoke(0, 0, 1, -1, 1);
  factory.GetSmoke(1, 2, 2, 0, 15);
  factory.GetSmoke(1, 2, 2, 0, 15);
  factory.GetSmoke(1, 2, 2, 0, 15);
  factory.GetSmoke(1, 2, 2, 0, 15);
  factory.GetSmoke(1, 2, 2, 0, 15);
  factory.GetSmoke(1, 2, 2, 0, 15);
  factory.GetSmoke(1, 2, 2, 0, 15);
  factory.GetSmoke(1, 2, 2, 0, 15);
}
