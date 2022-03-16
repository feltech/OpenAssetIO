#include <iostream>
#include <chrono>
#include <functional>
#include <random>
#include <string>
#include <algorithm>

#include <openassetio/specification/Specification.hpp>
#include <openassetio/trait/BlobTrait.hpp>
using namespace openassetio;

constexpr size_t nIterations = 100000;
const size_t dbSize = 1000000;

std::chrono::nanoseconds MeasureRuntime(std::function<void(void)> func)
{
  const auto start = std::chrono::high_resolution_clock::now();
  func();
  const auto stop = std::chrono::high_resolution_clock::now();
  return stop - start;
}

auto MeasureRuntimeMs(std::function<void(void)> func)
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(MeasureRuntime(func)).count();
}

///////////////////////////////////////////////////////////
std::string getEntityReference(size_t i) 
{
  return std::string("ams://myasset_") + std::to_string(i);
}

std::string getPrimaryString(size_t i) 
{
  return std::string("file://myasset_") + std::to_string(i);
}

///////////////////////////////////////////////////////////
class OldManagerInterface
{
public:
  OldManagerInterface(const std::unordered_map<std::string, std::string>& refToPrimStrs) : refToPrimStrs{refToPrimStrs} {}
  std::string resolveEntityReference(const std::string& ref);

private:
  const std::unordered_map<std::string, std::string>& refToPrimStrs;
};

std::string OldManagerInterface::resolveEntityReference(const std::string& ref)
{
  return refToPrimStrs.at(ref);
}

///////////////////////////////////////////////////////////
auto benchmarkOldManager(const std::unordered_map<std::string, std::string>& refToPrimStrs, const std::vector<std::string>& refs)
{
  OldManagerInterface oldMgr(refToPrimStrs);

  const auto doLookups = [&oldMgr, &refs]() {
    for(size_t i = 0; i < nIterations; ++i) {
      const auto& ref = refs[i];
      const auto& primStr = oldMgr.resolveEntityReference(ref);
      if(primStr.empty()) {
        std::cerr << "resolveEntityReference() incorrect result!" << std::endl;
        exit(-1);
      }
    }
  };

  return MeasureRuntimeMs(doLookups);
}

///////////////////////////////////////////////////////////
class NewManagerInterface
{
public:
  NewManagerInterface(const std::unordered_map<std::string, std::string>& refToPrimStrs) : refToPrimStrs{refToPrimStrs} {}
  std::shared_ptr<specification::Specification> resolve(const std::string& ref, const specification::Specification::TraitIds& traitIds);

private:
  const std::unordered_map<std::string, std::string>& refToPrimStrs;
};

std::shared_ptr<specification::Specification> NewManagerInterface::resolve(const std::string& ref, const specification::Specification::TraitIds& traitIds)
{
  specification::Specification::TraitIds populatedTraits;
  bool getPath = false;
  if(std::find(traitIds.cbegin(), traitIds.cend(), trait::BlobTrait::kId) != traitIds.cend()) {
    populatedTraits.emplace_back(trait::BlobTrait::kId);
    getPath = true;
  }
  auto result = std::make_shared<specification::Specification>(populatedTraits);
  if(getPath) {
    trait::BlobTrait btrait(result);
    btrait.setUrl(refToPrimStrs.at(ref));
  }
  return result;
}

///////////////////////////////////////////////////////////
auto benchmarkNewManager(const std::unordered_map<std::string, std::string>& refToPrimStrs, const std::vector<std::string>& refs)
{
  NewManagerInterface newMgr(refToPrimStrs);

  const auto doLookups = [&newMgr, &refs]() {
    for(size_t i = 0; i < nIterations; ++i) {
      const auto& ref = refs[i];
      const auto spec = newMgr.resolve(ref, {trait::BlobTrait::kId});
      trait::BlobTrait btrait(spec);
      trait::property::Str url;
      const auto success = btrait.getUrl(&url);
      if(success != trait::TraitPropertyStatus::kFound) {
        std::cerr << "getUrl() failed!" << std::endl;
        exit(-1);
      }
    }
  };

  return MeasureRuntimeMs(doLookups);
}

///////////////////////////////////////////////////////////
int main()
{
  std::cout << "CAUTION: on Windows do not run this in a Visual Studio command prompt "
    << "('x64 Native Tools Command Prompt for VS') or the measurements may fluctuate, "
    << "use a basic CMD shell" << std::endl;

  std::cout << "       Iterations: " << nIterations << std::endl;
  std::cout << "AMS database size: " << dbSize << std::endl;

  // Set up database
  std::unordered_map<std::string, std::string> refToPrimStrs;
  for(size_t i = 0; i < dbSize; ++i) {
    refToPrimStrs[getEntityReference(i)] = getPrimaryString(i);
  }

  // Set up a list of random refs we want to look up
  std::vector<std::string> refs;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> randAssetIndexDistrib(0, dbSize - 1);
  for(size_t i = 0; i < nIterations; ++i) {
    const auto rnd = randAssetIndexDistrib(gen);
    refs.emplace_back(getEntityReference(rnd));
  }

  const auto oldTime = benchmarkOldManager(refToPrimStrs, refs);
  std::cout << "resolveEntityReference: " << oldTime << " ms" << std::endl;

  const auto newTime = benchmarkNewManager(refToPrimStrs, refs);
  std::cout << "resolve: " << newTime << " ms" << std::endl;
}
