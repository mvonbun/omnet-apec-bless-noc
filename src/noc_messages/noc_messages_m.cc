//
// Generated file, do not edit! Created by nedtool 5.0 from noc_messages/noc_messages.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "noc_messages_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: no doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: no doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

TimeoutMsg_Base::TimeoutMsg_Base(const char *name, int kind) : ::omnetpp::cMessage(name,kind)
{
    this->setKind(TIMEOUT);

    this->packet_id = 0;
    this->timeout = 0;
}

TimeoutMsg_Base::TimeoutMsg_Base(const TimeoutMsg_Base& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

TimeoutMsg_Base::~TimeoutMsg_Base()
{
}

TimeoutMsg_Base& TimeoutMsg_Base::operator=(const TimeoutMsg_Base& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void TimeoutMsg_Base::copy(const TimeoutMsg_Base& other)
{
    this->packet_id = other.packet_id;
    this->timeout = other.timeout;
}

void TimeoutMsg_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->packet_id);
    doParsimPacking(b,this->timeout);
}

void TimeoutMsg_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->packet_id);
    doParsimUnpacking(b,this->timeout);
}

long TimeoutMsg_Base::getPacket_id() const
{
    return this->packet_id;
}

void TimeoutMsg_Base::setPacket_id(long packet_id)
{
    this->packet_id = packet_id;
}

::omnetpp::simtime_t TimeoutMsg_Base::getTimeout() const
{
    return this->timeout;
}

void TimeoutMsg_Base::setTimeout(::omnetpp::simtime_t timeout)
{
    this->timeout = timeout;
}

class TimeoutMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    TimeoutMsgDescriptor();
    virtual ~TimeoutMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(TimeoutMsgDescriptor);

TimeoutMsgDescriptor::TimeoutMsgDescriptor() : omnetpp::cClassDescriptor("TimeoutMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

TimeoutMsgDescriptor::~TimeoutMsgDescriptor()
{
    delete[] propertynames;
}

bool TimeoutMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TimeoutMsg_Base *>(obj)!=nullptr;
}

const char **TimeoutMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *TimeoutMsgDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int TimeoutMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int TimeoutMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *TimeoutMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "packet_id",
        "timeout",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int TimeoutMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_id")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "timeout")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *TimeoutMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "long",
        "simtime_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **TimeoutMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TimeoutMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TimeoutMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    TimeoutMsg_Base *pp = (TimeoutMsg_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string TimeoutMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    TimeoutMsg_Base *pp = (TimeoutMsg_Base *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getPacket_id());
        case 1: return simtime2string(pp->getTimeout());
        default: return "";
    }
}

bool TimeoutMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    TimeoutMsg_Base *pp = (TimeoutMsg_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setPacket_id(string2long(value)); return true;
        case 1: pp->setTimeout(string2simtime(value)); return true;
        default: return false;
    }
}

const char *TimeoutMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *TimeoutMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    TimeoutMsg_Base *pp = (TimeoutMsg_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

ResourceReleaseMsg_Base::ResourceReleaseMsg_Base(const char *name, int kind) : ::omnetpp::cMessage(name,kind)
{
    this->setKind(RELEVENT);

    this->message_id = 0;
    this->port_id = 0;
}

ResourceReleaseMsg_Base::ResourceReleaseMsg_Base(const ResourceReleaseMsg_Base& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ResourceReleaseMsg_Base::~ResourceReleaseMsg_Base()
{
}

ResourceReleaseMsg_Base& ResourceReleaseMsg_Base::operator=(const ResourceReleaseMsg_Base& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ResourceReleaseMsg_Base::copy(const ResourceReleaseMsg_Base& other)
{
    this->message_id = other.message_id;
    this->port_id = other.port_id;
}

void ResourceReleaseMsg_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->message_id);
    doParsimPacking(b,this->port_id);
}

void ResourceReleaseMsg_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->message_id);
    doParsimUnpacking(b,this->port_id);
}

long ResourceReleaseMsg_Base::getMessage_id() const
{
    return this->message_id;
}

void ResourceReleaseMsg_Base::setMessage_id(long message_id)
{
    this->message_id = message_id;
}

int ResourceReleaseMsg_Base::getPort_id() const
{
    return this->port_id;
}

void ResourceReleaseMsg_Base::setPort_id(int port_id)
{
    this->port_id = port_id;
}

class ResourceReleaseMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ResourceReleaseMsgDescriptor();
    virtual ~ResourceReleaseMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ResourceReleaseMsgDescriptor);

ResourceReleaseMsgDescriptor::ResourceReleaseMsgDescriptor() : omnetpp::cClassDescriptor("ResourceReleaseMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

ResourceReleaseMsgDescriptor::~ResourceReleaseMsgDescriptor()
{
    delete[] propertynames;
}

bool ResourceReleaseMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ResourceReleaseMsg_Base *>(obj)!=nullptr;
}

const char **ResourceReleaseMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ResourceReleaseMsgDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ResourceReleaseMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int ResourceReleaseMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *ResourceReleaseMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "message_id",
        "port_id",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int ResourceReleaseMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "message_id")==0) return base+0;
    if (fieldName[0]=='p' && strcmp(fieldName, "port_id")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ResourceReleaseMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "long",
        "int",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **ResourceReleaseMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ResourceReleaseMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ResourceReleaseMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ResourceReleaseMsg_Base *pp = (ResourceReleaseMsg_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ResourceReleaseMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ResourceReleaseMsg_Base *pp = (ResourceReleaseMsg_Base *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMessage_id());
        case 1: return long2string(pp->getPort_id());
        default: return "";
    }
}

bool ResourceReleaseMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ResourceReleaseMsg_Base *pp = (ResourceReleaseMsg_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setMessage_id(string2long(value)); return true;
        case 1: pp->setPort_id(string2long(value)); return true;
        default: return false;
    }
}

const char *ResourceReleaseMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *ResourceReleaseMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ResourceReleaseMsg_Base *pp = (ResourceReleaseMsg_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(PacketGenerationTriggerMsg);

PacketGenerationTriggerMsg::PacketGenerationTriggerMsg(const char *name, int kind) : ::omnetpp::cMessage(name,kind)
{
    this->setKind(PACKTRIG);
    this->setNum_base_flit(-1);
    this->setDestination(-1);
    this->setPriority(-1);

    this->num_base_flit = 0;
    this->destination = 0;
    this->priority = 0;
}

PacketGenerationTriggerMsg::PacketGenerationTriggerMsg(const PacketGenerationTriggerMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

PacketGenerationTriggerMsg::~PacketGenerationTriggerMsg()
{
}

PacketGenerationTriggerMsg& PacketGenerationTriggerMsg::operator=(const PacketGenerationTriggerMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void PacketGenerationTriggerMsg::copy(const PacketGenerationTriggerMsg& other)
{
    this->num_base_flit = other.num_base_flit;
    this->destination = other.destination;
    this->priority = other.priority;
}

void PacketGenerationTriggerMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->num_base_flit);
    doParsimPacking(b,this->destination);
    doParsimPacking(b,this->priority);
}

void PacketGenerationTriggerMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->num_base_flit);
    doParsimUnpacking(b,this->destination);
    doParsimUnpacking(b,this->priority);
}

int PacketGenerationTriggerMsg::getNum_base_flit() const
{
    return this->num_base_flit;
}

void PacketGenerationTriggerMsg::setNum_base_flit(int num_base_flit)
{
    this->num_base_flit = num_base_flit;
}

int PacketGenerationTriggerMsg::getDestination() const
{
    return this->destination;
}

void PacketGenerationTriggerMsg::setDestination(int destination)
{
    this->destination = destination;
}

int PacketGenerationTriggerMsg::getPriority() const
{
    return this->priority;
}

void PacketGenerationTriggerMsg::setPriority(int priority)
{
    this->priority = priority;
}

class PacketGenerationTriggerMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    PacketGenerationTriggerMsgDescriptor();
    virtual ~PacketGenerationTriggerMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(PacketGenerationTriggerMsgDescriptor);

PacketGenerationTriggerMsgDescriptor::PacketGenerationTriggerMsgDescriptor() : omnetpp::cClassDescriptor("PacketGenerationTriggerMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

PacketGenerationTriggerMsgDescriptor::~PacketGenerationTriggerMsgDescriptor()
{
    delete[] propertynames;
}

bool PacketGenerationTriggerMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<PacketGenerationTriggerMsg *>(obj)!=nullptr;
}

const char **PacketGenerationTriggerMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *PacketGenerationTriggerMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int PacketGenerationTriggerMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount() : 3;
}

unsigned int PacketGenerationTriggerMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *PacketGenerationTriggerMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "num_base_flit",
        "destination",
        "priority",
    };
    return (field>=0 && field<3) ? fieldNames[field] : nullptr;
}

int PacketGenerationTriggerMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "num_base_flit")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destination")==0) return base+1;
    if (fieldName[0]=='p' && strcmp(fieldName, "priority")==0) return base+2;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *PacketGenerationTriggerMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : nullptr;
}

const char **PacketGenerationTriggerMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *PacketGenerationTriggerMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int PacketGenerationTriggerMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    PacketGenerationTriggerMsg *pp = (PacketGenerationTriggerMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PacketGenerationTriggerMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    PacketGenerationTriggerMsg *pp = (PacketGenerationTriggerMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getNum_base_flit());
        case 1: return long2string(pp->getDestination());
        case 2: return long2string(pp->getPriority());
        default: return "";
    }
}

bool PacketGenerationTriggerMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    PacketGenerationTriggerMsg *pp = (PacketGenerationTriggerMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setNum_base_flit(string2long(value)); return true;
        case 1: pp->setDestination(string2long(value)); return true;
        case 2: pp->setPriority(string2long(value)); return true;
        default: return false;
    }
}

const char *PacketGenerationTriggerMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *PacketGenerationTriggerMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    PacketGenerationTriggerMsg *pp = (PacketGenerationTriggerMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(MonitorMsg);

MonitorMsg::MonitorMsg(const char *name, int kind) : ::omnetpp::cMessage(name,kind)
{
    this->setKind(MONITOR);

    this->module_id = 0;
    this->module_type = 0;
    this->value = 0;
}

MonitorMsg::MonitorMsg(const MonitorMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

MonitorMsg::~MonitorMsg()
{
}

MonitorMsg& MonitorMsg::operator=(const MonitorMsg& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void MonitorMsg::copy(const MonitorMsg& other)
{
    this->module_id = other.module_id;
    this->module_type = other.module_type;
    this->value = other.value;
}

void MonitorMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->module_id);
    doParsimPacking(b,this->module_type);
    doParsimPacking(b,this->value);
}

void MonitorMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->module_id);
    doParsimUnpacking(b,this->module_type);
    doParsimUnpacking(b,this->value);
}

int MonitorMsg::getModule_id() const
{
    return this->module_id;
}

void MonitorMsg::setModule_id(int module_id)
{
    this->module_id = module_id;
}

int MonitorMsg::getModule_type() const
{
    return this->module_type;
}

void MonitorMsg::setModule_type(int module_type)
{
    this->module_type = module_type;
}

int MonitorMsg::getValue() const
{
    return this->value;
}

void MonitorMsg::setValue(int value)
{
    this->value = value;
}

class MonitorMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    MonitorMsgDescriptor();
    virtual ~MonitorMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MonitorMsgDescriptor);

MonitorMsgDescriptor::MonitorMsgDescriptor() : omnetpp::cClassDescriptor("MonitorMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

MonitorMsgDescriptor::~MonitorMsgDescriptor()
{
    delete[] propertynames;
}

bool MonitorMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MonitorMsg *>(obj)!=nullptr;
}

const char **MonitorMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MonitorMsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MonitorMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount() : 3;
}

unsigned int MonitorMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *MonitorMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "module_id",
        "module_type",
        "value",
    };
    return (field>=0 && field<3) ? fieldNames[field] : nullptr;
}

int MonitorMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "module_id")==0) return base+0;
    if (fieldName[0]=='m' && strcmp(fieldName, "module_type")==0) return base+1;
    if (fieldName[0]=='v' && strcmp(fieldName, "value")==0) return base+2;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MonitorMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : nullptr;
}

const char **MonitorMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MonitorMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MonitorMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MonitorMsg *pp = (MonitorMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MonitorMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MonitorMsg *pp = (MonitorMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getModule_id());
        case 1: return long2string(pp->getModule_type());
        case 2: return long2string(pp->getValue());
        default: return "";
    }
}

bool MonitorMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MonitorMsg *pp = (MonitorMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setModule_id(string2long(value)); return true;
        case 1: pp->setModule_type(string2long(value)); return true;
        case 2: pp->setValue(string2long(value)); return true;
        default: return false;
    }
}

const char *MonitorMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *MonitorMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MonitorMsg *pp = (MonitorMsg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

PBuffOverflowMsg_Base::PBuffOverflowMsg_Base(const char *name, int kind) : ::omnetpp::cMessage(name,kind)
{
    this->setKind(CBUFOVFL);
    this->setSchedulingPriority(SP_LAST);

    this->module_id = 0;
}

PBuffOverflowMsg_Base::PBuffOverflowMsg_Base(const PBuffOverflowMsg_Base& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

PBuffOverflowMsg_Base::~PBuffOverflowMsg_Base()
{
}

PBuffOverflowMsg_Base& PBuffOverflowMsg_Base::operator=(const PBuffOverflowMsg_Base& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void PBuffOverflowMsg_Base::copy(const PBuffOverflowMsg_Base& other)
{
    this->module_id = other.module_id;
}

void PBuffOverflowMsg_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->module_id);
}

void PBuffOverflowMsg_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->module_id);
}

int PBuffOverflowMsg_Base::getModule_id() const
{
    return this->module_id;
}

void PBuffOverflowMsg_Base::setModule_id(int module_id)
{
    this->module_id = module_id;
}

class PBuffOverflowMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    PBuffOverflowMsgDescriptor();
    virtual ~PBuffOverflowMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(PBuffOverflowMsgDescriptor);

PBuffOverflowMsgDescriptor::PBuffOverflowMsgDescriptor() : omnetpp::cClassDescriptor("PBuffOverflowMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

PBuffOverflowMsgDescriptor::~PBuffOverflowMsgDescriptor()
{
    delete[] propertynames;
}

bool PBuffOverflowMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<PBuffOverflowMsg_Base *>(obj)!=nullptr;
}

const char **PBuffOverflowMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *PBuffOverflowMsgDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int PBuffOverflowMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int PBuffOverflowMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *PBuffOverflowMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "module_id",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int PBuffOverflowMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "module_id")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *PBuffOverflowMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : nullptr;
}

const char **PBuffOverflowMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *PBuffOverflowMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int PBuffOverflowMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    PBuffOverflowMsg_Base *pp = (PBuffOverflowMsg_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PBuffOverflowMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    PBuffOverflowMsg_Base *pp = (PBuffOverflowMsg_Base *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getModule_id());
        default: return "";
    }
}

bool PBuffOverflowMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    PBuffOverflowMsg_Base *pp = (PBuffOverflowMsg_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setModule_id(string2long(value)); return true;
        default: return false;
    }
}

const char *PBuffOverflowMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *PBuffOverflowMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    PBuffOverflowMsg_Base *pp = (PBuffOverflowMsg_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

ConvergenceMsg_Base::ConvergenceMsg_Base(const char *name, int kind) : ::omnetpp::cMessage(name,kind)
{
    this->setKind(CCONV);
    this->setSchedulingPriority(SP_LAST);

    this->module_id = 0;
    this->avg_latency = 0;
}

ConvergenceMsg_Base::ConvergenceMsg_Base(const ConvergenceMsg_Base& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ConvergenceMsg_Base::~ConvergenceMsg_Base()
{
}

ConvergenceMsg_Base& ConvergenceMsg_Base::operator=(const ConvergenceMsg_Base& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ConvergenceMsg_Base::copy(const ConvergenceMsg_Base& other)
{
    this->module_id = other.module_id;
    this->avg_latency = other.avg_latency;
}

void ConvergenceMsg_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->module_id);
    doParsimPacking(b,this->avg_latency);
}

void ConvergenceMsg_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->module_id);
    doParsimUnpacking(b,this->avg_latency);
}

int ConvergenceMsg_Base::getModule_id() const
{
    return this->module_id;
}

void ConvergenceMsg_Base::setModule_id(int module_id)
{
    this->module_id = module_id;
}

::omnetpp::simtime_t ConvergenceMsg_Base::getAvg_latency() const
{
    return this->avg_latency;
}

void ConvergenceMsg_Base::setAvg_latency(::omnetpp::simtime_t avg_latency)
{
    this->avg_latency = avg_latency;
}

class ConvergenceMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    ConvergenceMsgDescriptor();
    virtual ~ConvergenceMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(ConvergenceMsgDescriptor);

ConvergenceMsgDescriptor::ConvergenceMsgDescriptor() : omnetpp::cClassDescriptor("ConvergenceMsg", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

ConvergenceMsgDescriptor::~ConvergenceMsgDescriptor()
{
    delete[] propertynames;
}

bool ConvergenceMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ConvergenceMsg_Base *>(obj)!=nullptr;
}

const char **ConvergenceMsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ConvergenceMsgDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ConvergenceMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int ConvergenceMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *ConvergenceMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "module_id",
        "avg_latency",
    };
    return (field>=0 && field<2) ? fieldNames[field] : nullptr;
}

int ConvergenceMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "module_id")==0) return base+0;
    if (fieldName[0]=='a' && strcmp(fieldName, "avg_latency")==0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ConvergenceMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "simtime_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : nullptr;
}

const char **ConvergenceMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ConvergenceMsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ConvergenceMsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ConvergenceMsg_Base *pp = (ConvergenceMsg_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ConvergenceMsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ConvergenceMsg_Base *pp = (ConvergenceMsg_Base *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getModule_id());
        case 1: return simtime2string(pp->getAvg_latency());
        default: return "";
    }
}

bool ConvergenceMsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ConvergenceMsg_Base *pp = (ConvergenceMsg_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setModule_id(string2long(value)); return true;
        case 1: pp->setAvg_latency(string2simtime(value)); return true;
        default: return false;
    }
}

const char *ConvergenceMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *ConvergenceMsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ConvergenceMsg_Base *pp = (ConvergenceMsg_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


