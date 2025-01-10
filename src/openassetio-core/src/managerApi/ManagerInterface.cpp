// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2025 The Foundry Visionmongers Ltd
#include <cstddef>
#include <utility>

#include <fmt/format.h>

#include <openassetio/export.h>
#include <openassetio/EntityReference.hpp>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/access.hpp>
#include <openassetio/errors/exceptions.hpp>
#include <openassetio/hostApi/EntityReferencePager.hpp>
#include <openassetio/managerApi/EntityReferencePagerInterface.hpp>
#include <openassetio/managerApi/ManagerInterface.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/typedefs.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace managerApi {

ManagerInterface::ManagerInterface() = default;

#define UNIMPLEMENTED_ERROR(capability)                                                           \
  fmt::format(                                                                                    \
      "The '{}' method has not been implemented by the manager. Check manager capability for {} " \
      "by calling `manager.hasCapability`.",                                                      \
      __func__, ManagerInterface::kCapabilityNames[static_cast<std::size_t>(capability)])

InfoDictionary ManagerInterface::info() { return {}; }

// NOLINTNEXTLINE(performance-unnecessary-value-param)
void ManagerInterface::initialize(InfoDictionary managerSettings,
                                  [[maybe_unused]] const HostSessionPtr& hostSession) {
  if (!managerSettings.empty()) {
    throw errors::InputValidationException{
        "Settings provided but are not supported. The initialize method has not been implemented "
        "by the manager."};
  }
}

// NOLINTNEXTLINE(performance-unnecessary-value-param)
StrMap ManagerInterface::updateTerminology([[maybe_unused]] StrMap terms,
                                           [[maybe_unused]] const HostSessionPtr& hostSession) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kCustomTerminology)};
}

InfoDictionary ManagerInterface::settings([[maybe_unused]] const HostSessionPtr& hostSession) {
  return InfoDictionary{};
}

void ManagerInterface::flushCaches([[maybe_unused]] const HostSessionPtr& hostSession) {}

trait::TraitsDatas ManagerInterface::managementPolicy(
    [[maybe_unused]] const trait::TraitSets& traitSets,
    [[maybe_unused]] access::PolicyAccess policyAccess,
    [[maybe_unused]] const ContextConstPtr& context,
    [[maybe_unused]] const HostSessionPtr& hostSession) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kManagementPolicyQueries)};
}

void ManagerInterface::entityExists(
    [[maybe_unused]] const EntityReferences& entityReferences,
    [[maybe_unused]] const ContextConstPtr& context,
    [[maybe_unused]] const HostSessionPtr& hostSession,
    [[maybe_unused]] const ExistsSuccessCallback& successCallback,
    [[maybe_unused]] const BatchElementErrorCallback& errorCallback) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kExistenceQueries)};
}

void ManagerInterface::entityTraits(
    [[maybe_unused]] const EntityReferences& entityReferences,
    [[maybe_unused]] const access::EntityTraitsAccess entityTraitsAccess,
    [[maybe_unused]] const ContextConstPtr& context,
    [[maybe_unused]] const HostSessionPtr& hostSession,
    [[maybe_unused]] const EntityTraitsSuccessCallback& successCallback,
    [[maybe_unused]] const BatchElementErrorCallback& errorCallback) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kEntityTraitIntrospection)};
}

void ManagerInterface::resolve([[maybe_unused]] const EntityReferences& entityReferences,
                               [[maybe_unused]] const trait::TraitSet& traitSet,
                               [[maybe_unused]] access::ResolveAccess resolveAccess,
                               [[maybe_unused]] const ContextConstPtr& context,
                               [[maybe_unused]] const HostSessionPtr& hostSession,
                               [[maybe_unused]] const ResolveSuccessCallback& successCallback,
                               [[maybe_unused]] const BatchElementErrorCallback& errorCallback) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kResolution)};
}

ManagerStateBasePtr ManagerInterface::createState(
    [[maybe_unused]] const HostSessionPtr& hostSession) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kStatefulContexts)};
}

ManagerStateBasePtr ManagerInterface::createChildState(
    [[maybe_unused]] const ManagerStateBasePtr& parentState,
    [[maybe_unused]] const HostSessionPtr& hostSession) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kStatefulContexts)};
}

Str ManagerInterface::persistenceTokenForState(
    [[maybe_unused]] const ManagerStateBasePtr& state,
    [[maybe_unused]] const HostSessionPtr& hostSession) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kStatefulContexts)};
}

ManagerStateBasePtr ManagerInterface::stateFromPersistenceToken(
    [[maybe_unused]] const Str& token, [[maybe_unused]] const HostSessionPtr& hostSession) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kStatefulContexts)};
}

bool ManagerInterface::isEntityReferenceString(
    [[maybe_unused]] const Str& someString, [[maybe_unused]] const HostSessionPtr& hostSession) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kEntityReferenceIdentification)};
}

// To avoid changing this to non-static in the not too distant, when we
// add manager validation (see https://github.com/OpenAssetIO/OpenAssetIO/issues/553).
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
EntityReference ManagerInterface::createEntityReference(Str entityReferenceString) const {
  return EntityReference(std::move(entityReferenceString));
}

void ManagerInterface::defaultEntityReference(
    [[maybe_unused]] const trait::TraitSets& traitSets,
    [[maybe_unused]] const access::DefaultEntityAccess defaultEntityAccess,
    [[maybe_unused]] const ContextConstPtr& context,
    [[maybe_unused]] const HostSessionPtr& hostSession,
    [[maybe_unused]] const DefaultEntityReferenceSuccessCallback& successCallback,
    [[maybe_unused]] const BatchElementErrorCallback& errorCallback) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kDefaultEntityReferences)};
}

void ManagerInterface::getWithRelationship(
    [[maybe_unused]] const EntityReferences& entityReferences,
    [[maybe_unused]] const trait::TraitsDataPtr& relationshipTraitsData,
    [[maybe_unused]] const trait::TraitSet& resultTraitSet, [[maybe_unused]] size_t pageSize,
    [[maybe_unused]] const access::RelationsAccess relationsAccess,
    [[maybe_unused]] const ContextConstPtr& context,
    [[maybe_unused]] const HostSessionPtr& hostSession,
    [[maybe_unused]] const RelationshipQuerySuccessCallback& successCallback,
    [[maybe_unused]] const BatchElementErrorCallback& errorCallback) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kRelationshipQueries)};
}

void ManagerInterface::getWithRelationships(
    [[maybe_unused]] const EntityReference& entityReference,
    [[maybe_unused]] const trait::TraitsDatas& relationshipTraitsDatas,
    [[maybe_unused]] const trait::TraitSet& resultTraitSet, [[maybe_unused]] size_t pageSize,
    [[maybe_unused]] const access::RelationsAccess relationsAccess,
    [[maybe_unused]] const ContextConstPtr& context,
    [[maybe_unused]] const HostSessionPtr& hostSession,
    [[maybe_unused]] const RelationshipQuerySuccessCallback& successCallback,
    [[maybe_unused]] const BatchElementErrorCallback& errorCallback) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kRelationshipQueries)};
}

void ManagerInterface::preflight([[maybe_unused]] const EntityReferences& entityReferences,
                                 [[maybe_unused]] const trait::TraitsDatas& traitsHints,
                                 [[maybe_unused]] access::PublishingAccess publishingAccess,
                                 [[maybe_unused]] const ContextConstPtr& context,
                                 [[maybe_unused]] const HostSessionPtr& hostSession,
                                 [[maybe_unused]] const PreflightSuccessCallback& successCallback,
                                 [[maybe_unused]] const BatchElementErrorCallback& errorCallback) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kPublishing)};
}

void ManagerInterface::register_([[maybe_unused]] const EntityReferences& entityReferences,
                                 [[maybe_unused]] const trait::TraitsDatas& entityTraitsDatas,
                                 [[maybe_unused]] access::PublishingAccess publishingAccess,
                                 [[maybe_unused]] const ContextConstPtr& context,
                                 [[maybe_unused]] const HostSessionPtr& hostSession,
                                 [[maybe_unused]] const RegisterSuccessCallback& successCallback,
                                 [[maybe_unused]] const BatchElementErrorCallback& errorCallback) {
  throw errors::NotImplementedException{
      UNIMPLEMENTED_ERROR(ManagerInterface::Capability::kPublishing)};
}

}  // namespace managerApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
