/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: msp/msp_principal.proto */

#ifndef PROTOBUF_C_msp_2fmsp_5fprincipal_2eproto__INCLUDED
#define PROTOBUF_C_msp_2fmsp_5fprincipal_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1002001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Common__MSPPrincipal Common__MSPPrincipal;
typedef struct _Common__OrganizationUnit Common__OrganizationUnit;
typedef struct _Common__MSPRole Common__MSPRole;
typedef struct _Common__MSPIdentityAnonymity Common__MSPIdentityAnonymity;
typedef struct _Common__CombinedPrincipal Common__CombinedPrincipal;


/* --- enums --- */

typedef enum _Common__MSPPrincipal__Classification {
  /*
   * Represents the one of the dedicated MSP roles, the
   */
  COMMON__MSPPRINCIPAL__CLASSIFICATION__ROLE = 0,
  /*
   * one of a member of MSP network, and the one of an
   * administrator of an MSP network
   */
  /*
   * Denotes a finer grained (affiliation-based)
   */
  COMMON__MSPPRINCIPAL__CLASSIFICATION__ORGANIZATION_UNIT = 1,
  /*
   * groupping of entities, per MSP affiliation
   * E.g., this can well be represented by an MSP's
   * Organization unit
   */
  /*
   * Denotes a principal that consists of a single
   */
  COMMON__MSPPRINCIPAL__CLASSIFICATION__IDENTITY = 2,
  /*
   * identity
   */
  /*
   * Denotes a principal that can be used to enforce
   */
  COMMON__MSPPRINCIPAL__CLASSIFICATION__ANONYMITY = 3,
  /*
   * an identity to be anonymous or nominal.
   */
  /*
   * Denotes a combined principal
   */
  COMMON__MSPPRINCIPAL__CLASSIFICATION__COMBINED = 4
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(COMMON__MSPPRINCIPAL__CLASSIFICATION)
} Common__MSPPrincipal__Classification;
typedef enum _Common__MSPRole__MSPRoleType {
  /*
   * Represents an MSP Member
   */
  COMMON__MSPROLE__MSPROLE_TYPE__MEMBER = 0,
  /*
   * Represents an MSP Admin
   */
  COMMON__MSPROLE__MSPROLE_TYPE__ADMIN = 1,
  /*
   * Represents an MSP Client
   */
  COMMON__MSPROLE__MSPROLE_TYPE__CLIENT = 2,
  /*
   * Represents an MSP Peer
   */
  COMMON__MSPROLE__MSPROLE_TYPE__PEER = 3
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(COMMON__MSPROLE__MSPROLE_TYPE)
} Common__MSPRole__MSPRoleType;
typedef enum _Common__MSPIdentityAnonymity__MSPIdentityAnonymityType {
  /*
   * Represents a nominal MSP Identity
   */
  COMMON__MSPIDENTITY_ANONYMITY__MSPIDENTITY_ANONYMITY_TYPE__NOMINAL = 0,
  /*
   * Represents an anonymous MSP Identity
   */
  COMMON__MSPIDENTITY_ANONYMITY__MSPIDENTITY_ANONYMITY_TYPE__ANONYMOUS = 1
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(COMMON__MSPIDENTITY_ANONYMITY__MSPIDENTITY_ANONYMITY_TYPE)
} Common__MSPIdentityAnonymity__MSPIdentityAnonymityType;

/* --- messages --- */

/*
 * MSPPrincipal aims to represent an MSP-centric set of identities.
 * In particular, this structure allows for definition of
 *  - a group of identities that are member of the same MSP
 *  - a group of identities that are member of the same organization unit
 *    in the same MSP
 *  - a group of identities that are administering a specific MSP
 *  - a specific identity
 * Expressing these groups is done given two fields of the fields below
 *  - Classification, that defines the type of classification of identities
 *    in an MSP this principal would be defined on; Classification can take
 *    three values:
 *     (i)  ByMSPRole: that represents a classification of identities within
 *          MSP based on one of the two pre-defined MSP rules, "member" and "admin"
 *     (ii) ByOrganizationUnit: that represents a classification of identities
 *          within MSP based on the organization unit an identity belongs to
 *     (iii)ByIdentity that denotes that MSPPrincipal is mapped to a single
 *          identity/certificate; this would mean that the Principal bytes
 *          message
 */
struct  _Common__MSPPrincipal
{
  ProtobufCMessage base;
  /*
   * Classification describes the way that one should process
   * Principal. An Classification value of "ByOrganizationUnit" reflects
   * that "Principal" contains the name of an organization this MSP
   * handles. A Classification value "ByIdentity" means that
   * "Principal" contains a specific identity. Default value
   * denotes that Principal contains one of the groups by
   * default supported by all MSPs ("admin" or "member").
   */
  protobuf_c_boolean has_principal_classification;
  Common__MSPPrincipal__Classification principal_classification;
  /*
   * Principal completes the policy principal definition. For the default
   * principal types, Principal can be either "Admin" or "Member".
   * For the ByOrganizationUnit/ByIdentity values of Classification,
   * PolicyPrincipal acquires its value from an organization unit or
   * identity, respectively.
   * For the Combined Classification type, the Principal is a marshalled
   * CombinedPrincipal.
   */
  protobuf_c_boolean has_principal;
  ProtobufCBinaryData principal;
};
#define COMMON__MSPPRINCIPAL__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&common__mspprincipal__descriptor) \
    , 0,0, 0,{0,NULL} }


/*
 * OrganizationUnit governs the organization of the Principal
 * field of a policy principal when a specific organization unity members
 * are to be defined within a policy principal.
 */
struct  _Common__OrganizationUnit
{
  ProtobufCMessage base;
  /*
   * MSPIdentifier represents the identifier of the MSP this organization unit
   * refers to
   */
  char *msp_identifier;
  /*
   * OrganizationUnitIdentifier defines the organizational unit under the
   * MSP identified with MSPIdentifier
   */
  char *organizational_unit_identifier;
  /*
   * CertifiersIdentifier is the hash of certificates chain of trust
   * related to this organizational unit
   */
  protobuf_c_boolean has_certifiers_identifier;
  ProtobufCBinaryData certifiers_identifier;
};
#define COMMON__ORGANIZATION_UNIT__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&common__organization_unit__descriptor) \
    , NULL, NULL, 0,{0,NULL} }


/*
 * MSPRole governs the organization of the Principal
 * field of an MSPPrincipal when it aims to define one of the
 * two dedicated roles within an MSP: Admin and Members.
 */
struct  _Common__MSPRole
{
  ProtobufCMessage base;
  /*
   * MSPIdentifier represents the identifier of the MSP this principal
   * refers to
   */
  char *msp_identifier;
  /*
   * MSPRoleType defines which of the available, pre-defined MSP-roles
   * an identiy should posess inside the MSP with identifier MSPidentifier
   */
  protobuf_c_boolean has_role;
  Common__MSPRole__MSPRoleType role;
};
#define COMMON__MSPROLE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&common__msprole__descriptor) \
    , NULL, 0,0 }


/*
 * MSPIdentityAnonymity can be used to enforce an identity to be anonymous or nominal.
 */
struct  _Common__MSPIdentityAnonymity
{
  ProtobufCMessage base;
  protobuf_c_boolean has_anonymity_type;
  Common__MSPIdentityAnonymity__MSPIdentityAnonymityType anonymity_type;
};
#define COMMON__MSPIDENTITY_ANONYMITY__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&common__mspidentity_anonymity__descriptor) \
    , 0,0 }


/*
 * CombinedPrincipal governs the organization of the Principal
 * field of a policy principal when principal_classification has
 * indicated that a combined form of principals is required
 */
struct  _Common__CombinedPrincipal
{
  ProtobufCMessage base;
  /*
   * Principals refer to combined principals
   */
  size_t n_principals;
  Common__MSPPrincipal **principals;
};
#define COMMON__COMBINED_PRINCIPAL__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&common__combined_principal__descriptor) \
    , 0,NULL }


/* Common__MSPPrincipal methods */
void   common__mspprincipal__init
                     (Common__MSPPrincipal         *message);
size_t common__mspprincipal__get_packed_size
                     (const Common__MSPPrincipal   *message);
size_t common__mspprincipal__pack
                     (const Common__MSPPrincipal   *message,
                      uint8_t             *out);
size_t common__mspprincipal__pack_to_buffer
                     (const Common__MSPPrincipal   *message,
                      ProtobufCBuffer     *buffer);
Common__MSPPrincipal *
       common__mspprincipal__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   common__mspprincipal__free_unpacked
                     (Common__MSPPrincipal *message,
                      ProtobufCAllocator *allocator);
/* Common__OrganizationUnit methods */
void   common__organization_unit__init
                     (Common__OrganizationUnit         *message);
size_t common__organization_unit__get_packed_size
                     (const Common__OrganizationUnit   *message);
size_t common__organization_unit__pack
                     (const Common__OrganizationUnit   *message,
                      uint8_t             *out);
size_t common__organization_unit__pack_to_buffer
                     (const Common__OrganizationUnit   *message,
                      ProtobufCBuffer     *buffer);
Common__OrganizationUnit *
       common__organization_unit__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   common__organization_unit__free_unpacked
                     (Common__OrganizationUnit *message,
                      ProtobufCAllocator *allocator);
/* Common__MSPRole methods */
void   common__msprole__init
                     (Common__MSPRole         *message);
size_t common__msprole__get_packed_size
                     (const Common__MSPRole   *message);
size_t common__msprole__pack
                     (const Common__MSPRole   *message,
                      uint8_t             *out);
size_t common__msprole__pack_to_buffer
                     (const Common__MSPRole   *message,
                      ProtobufCBuffer     *buffer);
Common__MSPRole *
       common__msprole__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   common__msprole__free_unpacked
                     (Common__MSPRole *message,
                      ProtobufCAllocator *allocator);
/* Common__MSPIdentityAnonymity methods */
void   common__mspidentity_anonymity__init
                     (Common__MSPIdentityAnonymity         *message);
size_t common__mspidentity_anonymity__get_packed_size
                     (const Common__MSPIdentityAnonymity   *message);
size_t common__mspidentity_anonymity__pack
                     (const Common__MSPIdentityAnonymity   *message,
                      uint8_t             *out);
size_t common__mspidentity_anonymity__pack_to_buffer
                     (const Common__MSPIdentityAnonymity   *message,
                      ProtobufCBuffer     *buffer);
Common__MSPIdentityAnonymity *
       common__mspidentity_anonymity__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   common__mspidentity_anonymity__free_unpacked
                     (Common__MSPIdentityAnonymity *message,
                      ProtobufCAllocator *allocator);
/* Common__CombinedPrincipal methods */
void   common__combined_principal__init
                     (Common__CombinedPrincipal         *message);
size_t common__combined_principal__get_packed_size
                     (const Common__CombinedPrincipal   *message);
size_t common__combined_principal__pack
                     (const Common__CombinedPrincipal   *message,
                      uint8_t             *out);
size_t common__combined_principal__pack_to_buffer
                     (const Common__CombinedPrincipal   *message,
                      ProtobufCBuffer     *buffer);
Common__CombinedPrincipal *
       common__combined_principal__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   common__combined_principal__free_unpacked
                     (Common__CombinedPrincipal *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Common__MSPPrincipal_Closure)
                 (const Common__MSPPrincipal *message,
                  void *closure_data);
typedef void (*Common__OrganizationUnit_Closure)
                 (const Common__OrganizationUnit *message,
                  void *closure_data);
typedef void (*Common__MSPRole_Closure)
                 (const Common__MSPRole *message,
                  void *closure_data);
typedef void (*Common__MSPIdentityAnonymity_Closure)
                 (const Common__MSPIdentityAnonymity *message,
                  void *closure_data);
typedef void (*Common__CombinedPrincipal_Closure)
                 (const Common__CombinedPrincipal *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor common__mspprincipal__descriptor;
extern const ProtobufCEnumDescriptor    common__mspprincipal__classification__descriptor;
extern const ProtobufCMessageDescriptor common__organization_unit__descriptor;
extern const ProtobufCMessageDescriptor common__msprole__descriptor;
extern const ProtobufCEnumDescriptor    common__msprole__msprole_type__descriptor;
extern const ProtobufCMessageDescriptor common__mspidentity_anonymity__descriptor;
extern const ProtobufCEnumDescriptor    common__mspidentity_anonymity__mspidentity_anonymity_type__descriptor;
extern const ProtobufCMessageDescriptor common__combined_principal__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_msp_2fmsp_5fprincipal_2eproto__INCLUDED */
