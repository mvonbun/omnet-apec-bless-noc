//
// Generated file, do not edit! Created by nedtool 5.0 from noc_messages/noc_flits.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "noc_flits_m.h"

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

Register_Class(RootFlit);

RootFlit::RootFlit(const char *name, int kind) : ::omnetpp::cPacket(name,kind)
{
    this->setSchedulingPriority(SP_BASE);

    this->sequence_number = 0;
}

RootFlit::RootFlit(const RootFlit& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

RootFlit::~RootFlit()
{
}

RootFlit& RootFlit::operator=(const RootFlit& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void RootFlit::copy(const RootFlit& other)
{
    this->sequence_number = other.sequence_number;
}

void RootFlit::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->sequence_number);
}

void RootFlit::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->sequence_number);
}

int RootFlit::getSequence_number() const
{
    return this->sequence_number;
}

void RootFlit::setSequence_number(int sequence_number)
{
    this->sequence_number = sequence_number;
}

class RootFlitDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RootFlitDescriptor();
    virtual ~RootFlitDescriptor();

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

Register_ClassDescriptor(RootFlitDescriptor);

RootFlitDescriptor::RootFlitDescriptor() : omnetpp::cClassDescriptor("RootFlit", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

RootFlitDescriptor::~RootFlitDescriptor()
{
    delete[] propertynames;
}

bool RootFlitDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RootFlit *>(obj)!=nullptr;
}

const char **RootFlitDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RootFlitDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RootFlitDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int RootFlitDescriptor::getFieldTypeFlags(int field) const
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

const char *RootFlitDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sequence_number",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int RootFlitDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sequence_number")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RootFlitDescriptor::getFieldTypeString(int field) const
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

const char **RootFlitDescriptor::getFieldPropertyNames(int field) const
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

const char *RootFlitDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int RootFlitDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RootFlit *pp = (RootFlit *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string RootFlitDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RootFlit *pp = (RootFlit *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSequence_number());
        default: return "";
    }
}

bool RootFlitDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RootFlit *pp = (RootFlit *)object; (void)pp;
    switch (field) {
        case 0: pp->setSequence_number(string2long(value)); return true;
        default: return false;
    }
}

const char *RootFlitDescriptor::getFieldStructName(int field) const
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

void *RootFlitDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RootFlit *pp = (RootFlit *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

BodyFlit_Base::BodyFlit_Base(const char *name, int kind) : ::RootFlit(name,kind)
{
    this->setKind(BODY);
}

BodyFlit_Base::BodyFlit_Base(const BodyFlit_Base& other) : ::RootFlit(other)
{
    copy(other);
}

BodyFlit_Base::~BodyFlit_Base()
{
}

BodyFlit_Base& BodyFlit_Base::operator=(const BodyFlit_Base& other)
{
    if (this==&other) return *this;
    ::RootFlit::operator=(other);
    copy(other);
    return *this;
}

void BodyFlit_Base::copy(const BodyFlit_Base& other)
{
}

void BodyFlit_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::RootFlit::parsimPack(b);
}

void BodyFlit_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::RootFlit::parsimUnpack(b);
}

class BodyFlitDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    BodyFlitDescriptor();
    virtual ~BodyFlitDescriptor();

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

Register_ClassDescriptor(BodyFlitDescriptor);

BodyFlitDescriptor::BodyFlitDescriptor() : omnetpp::cClassDescriptor("BodyFlit", "RootFlit")
{
    propertynames = nullptr;
}

BodyFlitDescriptor::~BodyFlitDescriptor()
{
    delete[] propertynames;
}

bool BodyFlitDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<BodyFlit_Base *>(obj)!=nullptr;
}

const char **BodyFlitDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *BodyFlitDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int BodyFlitDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int BodyFlitDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *BodyFlitDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int BodyFlitDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *BodyFlitDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **BodyFlitDescriptor::getFieldPropertyNames(int field) const
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

const char *BodyFlitDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int BodyFlitDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    BodyFlit_Base *pp = (BodyFlit_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string BodyFlitDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    BodyFlit_Base *pp = (BodyFlit_Base *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool BodyFlitDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    BodyFlit_Base *pp = (BodyFlit_Base *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *BodyFlitDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *BodyFlitDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    BodyFlit_Base *pp = (BodyFlit_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

TailFlit_Base::TailFlit_Base(const char *name, int kind) : ::RootFlit(name,kind)
{
    this->setKind(TAIL);
}

TailFlit_Base::TailFlit_Base(const TailFlit_Base& other) : ::RootFlit(other)
{
    copy(other);
}

TailFlit_Base::~TailFlit_Base()
{
}

TailFlit_Base& TailFlit_Base::operator=(const TailFlit_Base& other)
{
    if (this==&other) return *this;
    ::RootFlit::operator=(other);
    copy(other);
    return *this;
}

void TailFlit_Base::copy(const TailFlit_Base& other)
{
}

void TailFlit_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::RootFlit::parsimPack(b);
}

void TailFlit_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::RootFlit::parsimUnpack(b);
}

class TailFlitDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    TailFlitDescriptor();
    virtual ~TailFlitDescriptor();

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

Register_ClassDescriptor(TailFlitDescriptor);

TailFlitDescriptor::TailFlitDescriptor() : omnetpp::cClassDescriptor("TailFlit", "RootFlit")
{
    propertynames = nullptr;
}

TailFlitDescriptor::~TailFlitDescriptor()
{
    delete[] propertynames;
}

bool TailFlitDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TailFlit_Base *>(obj)!=nullptr;
}

const char **TailFlitDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *TailFlitDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int TailFlitDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int TailFlitDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *TailFlitDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int TailFlitDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *TailFlitDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **TailFlitDescriptor::getFieldPropertyNames(int field) const
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

const char *TailFlitDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int TailFlitDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    TailFlit_Base *pp = (TailFlit_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string TailFlitDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    TailFlit_Base *pp = (TailFlit_Base *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool TailFlitDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    TailFlit_Base *pp = (TailFlit_Base *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *TailFlitDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *TailFlitDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    TailFlit_Base *pp = (TailFlit_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

HeadFlit_Base::HeadFlit_Base(const char *name, int kind) : ::RootFlit(name,kind)
{
    this->setKind(HEAD);
    this->setSequence_number(0);

    this->source = 0;
    this->destination = 0;
    this->generation_time = 0;
    this->injection_time = 0;
    this->packet_id = 0;
    this->message_id = 0;
    this->priority = 0;
    this->num_flit = 0;
    this->num_baseflit = 0;
    this->num_codeflit = 0;
    this->timeout = 0;
    this->attempts = 0;
    this->distance = 0;
}

HeadFlit_Base::HeadFlit_Base(const HeadFlit_Base& other) : ::RootFlit(other)
{
    copy(other);
}

HeadFlit_Base::~HeadFlit_Base()
{
}

HeadFlit_Base& HeadFlit_Base::operator=(const HeadFlit_Base& other)
{
    if (this==&other) return *this;
    ::RootFlit::operator=(other);
    copy(other);
    return *this;
}

void HeadFlit_Base::copy(const HeadFlit_Base& other)
{
    this->source = other.source;
    this->destination = other.destination;
    this->generation_time = other.generation_time;
    this->injection_time = other.injection_time;
    this->packet_id = other.packet_id;
    this->message_id = other.message_id;
    this->priority = other.priority;
    this->num_flit = other.num_flit;
    this->num_baseflit = other.num_baseflit;
    this->num_codeflit = other.num_codeflit;
    this->timeout = other.timeout;
    this->attempts = other.attempts;
    this->distance = other.distance;
}

void HeadFlit_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::RootFlit::parsimPack(b);
    doParsimPacking(b,this->source);
    doParsimPacking(b,this->destination);
    doParsimPacking(b,this->generation_time);
    doParsimPacking(b,this->injection_time);
    doParsimPacking(b,this->packet_id);
    doParsimPacking(b,this->message_id);
    doParsimPacking(b,this->priority);
    doParsimPacking(b,this->num_flit);
    doParsimPacking(b,this->num_baseflit);
    doParsimPacking(b,this->num_codeflit);
    doParsimPacking(b,this->timeout);
    doParsimPacking(b,this->attempts);
    doParsimPacking(b,this->distance);
}

void HeadFlit_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::RootFlit::parsimUnpack(b);
    doParsimUnpacking(b,this->source);
    doParsimUnpacking(b,this->destination);
    doParsimUnpacking(b,this->generation_time);
    doParsimUnpacking(b,this->injection_time);
    doParsimUnpacking(b,this->packet_id);
    doParsimUnpacking(b,this->message_id);
    doParsimUnpacking(b,this->priority);
    doParsimUnpacking(b,this->num_flit);
    doParsimUnpacking(b,this->num_baseflit);
    doParsimUnpacking(b,this->num_codeflit);
    doParsimUnpacking(b,this->timeout);
    doParsimUnpacking(b,this->attempts);
    doParsimUnpacking(b,this->distance);
}

int HeadFlit_Base::getSource() const
{
    return this->source;
}

void HeadFlit_Base::setSource(int source)
{
    this->source = source;
}

int HeadFlit_Base::getDestination() const
{
    return this->destination;
}

void HeadFlit_Base::setDestination(int destination)
{
    this->destination = destination;
}

::omnetpp::simtime_t HeadFlit_Base::getGeneration_time() const
{
    return this->generation_time;
}

void HeadFlit_Base::setGeneration_time(::omnetpp::simtime_t generation_time)
{
    this->generation_time = generation_time;
}

::omnetpp::simtime_t HeadFlit_Base::getInjection_time() const
{
    return this->injection_time;
}

void HeadFlit_Base::setInjection_time(::omnetpp::simtime_t injection_time)
{
    this->injection_time = injection_time;
}

long HeadFlit_Base::getPacket_id() const
{
    return this->packet_id;
}

void HeadFlit_Base::setPacket_id(long packet_id)
{
    this->packet_id = packet_id;
}

long HeadFlit_Base::getMessage_id() const
{
    return this->message_id;
}

void HeadFlit_Base::setMessage_id(long message_id)
{
    this->message_id = message_id;
}

int HeadFlit_Base::getPriority() const
{
    return this->priority;
}

void HeadFlit_Base::setPriority(int priority)
{
    this->priority = priority;
}

int HeadFlit_Base::getNum_flit() const
{
    return this->num_flit;
}

void HeadFlit_Base::setNum_flit(int num_flit)
{
    this->num_flit = num_flit;
}

int HeadFlit_Base::getNum_baseflit() const
{
    return this->num_baseflit;
}

void HeadFlit_Base::setNum_baseflit(int num_baseflit)
{
    this->num_baseflit = num_baseflit;
}

int HeadFlit_Base::getNum_codeflit() const
{
    return this->num_codeflit;
}

void HeadFlit_Base::setNum_codeflit(int num_codeflit)
{
    this->num_codeflit = num_codeflit;
}

::omnetpp::simtime_t HeadFlit_Base::getTimeout() const
{
    return this->timeout;
}

void HeadFlit_Base::setTimeout(::omnetpp::simtime_t timeout)
{
    this->timeout = timeout;
}

int HeadFlit_Base::getAttempts() const
{
    return this->attempts;
}

void HeadFlit_Base::setAttempts(int attempts)
{
    this->attempts = attempts;
}

int HeadFlit_Base::getDistance() const
{
    return this->distance;
}

void HeadFlit_Base::setDistance(int distance)
{
    this->distance = distance;
}

class HeadFlitDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    HeadFlitDescriptor();
    virtual ~HeadFlitDescriptor();

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

Register_ClassDescriptor(HeadFlitDescriptor);

HeadFlitDescriptor::HeadFlitDescriptor() : omnetpp::cClassDescriptor("HeadFlit", "RootFlit")
{
    propertynames = nullptr;
}

HeadFlitDescriptor::~HeadFlitDescriptor()
{
    delete[] propertynames;
}

bool HeadFlitDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<HeadFlit_Base *>(obj)!=nullptr;
}

const char **HeadFlitDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *HeadFlitDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int HeadFlitDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 13+basedesc->getFieldCount() : 13;
}

unsigned int HeadFlitDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<13) ? fieldTypeFlags[field] : 0;
}

const char *HeadFlitDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "source",
        "destination",
        "generation_time",
        "injection_time",
        "packet_id",
        "message_id",
        "priority",
        "num_flit",
        "num_baseflit",
        "num_codeflit",
        "timeout",
        "attempts",
        "distance",
    };
    return (field>=0 && field<13) ? fieldNames[field] : nullptr;
}

int HeadFlitDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "source")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destination")==0) return base+1;
    if (fieldName[0]=='g' && strcmp(fieldName, "generation_time")==0) return base+2;
    if (fieldName[0]=='i' && strcmp(fieldName, "injection_time")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_id")==0) return base+4;
    if (fieldName[0]=='m' && strcmp(fieldName, "message_id")==0) return base+5;
    if (fieldName[0]=='p' && strcmp(fieldName, "priority")==0) return base+6;
    if (fieldName[0]=='n' && strcmp(fieldName, "num_flit")==0) return base+7;
    if (fieldName[0]=='n' && strcmp(fieldName, "num_baseflit")==0) return base+8;
    if (fieldName[0]=='n' && strcmp(fieldName, "num_codeflit")==0) return base+9;
    if (fieldName[0]=='t' && strcmp(fieldName, "timeout")==0) return base+10;
    if (fieldName[0]=='a' && strcmp(fieldName, "attempts")==0) return base+11;
    if (fieldName[0]=='d' && strcmp(fieldName, "distance")==0) return base+12;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *HeadFlitDescriptor::getFieldTypeString(int field) const
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
        "simtime_t",
        "simtime_t",
        "long",
        "long",
        "int",
        "int",
        "int",
        "int",
        "simtime_t",
        "int",
        "int",
    };
    return (field>=0 && field<13) ? fieldTypeStrings[field] : nullptr;
}

const char **HeadFlitDescriptor::getFieldPropertyNames(int field) const
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

const char *HeadFlitDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int HeadFlitDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    HeadFlit_Base *pp = (HeadFlit_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string HeadFlitDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    HeadFlit_Base *pp = (HeadFlit_Base *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSource());
        case 1: return long2string(pp->getDestination());
        case 2: return simtime2string(pp->getGeneration_time());
        case 3: return simtime2string(pp->getInjection_time());
        case 4: return long2string(pp->getPacket_id());
        case 5: return long2string(pp->getMessage_id());
        case 6: return long2string(pp->getPriority());
        case 7: return long2string(pp->getNum_flit());
        case 8: return long2string(pp->getNum_baseflit());
        case 9: return long2string(pp->getNum_codeflit());
        case 10: return simtime2string(pp->getTimeout());
        case 11: return long2string(pp->getAttempts());
        case 12: return long2string(pp->getDistance());
        default: return "";
    }
}

bool HeadFlitDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    HeadFlit_Base *pp = (HeadFlit_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setSource(string2long(value)); return true;
        case 1: pp->setDestination(string2long(value)); return true;
        case 2: pp->setGeneration_time(string2simtime(value)); return true;
        case 3: pp->setInjection_time(string2simtime(value)); return true;
        case 4: pp->setPacket_id(string2long(value)); return true;
        case 5: pp->setMessage_id(string2long(value)); return true;
        case 6: pp->setPriority(string2long(value)); return true;
        case 7: pp->setNum_flit(string2long(value)); return true;
        case 8: pp->setNum_baseflit(string2long(value)); return true;
        case 9: pp->setNum_codeflit(string2long(value)); return true;
        case 10: pp->setTimeout(string2simtime(value)); return true;
        case 11: pp->setAttempts(string2long(value)); return true;
        case 12: pp->setDistance(string2long(value)); return true;
        default: return false;
    }
}

const char *HeadFlitDescriptor::getFieldStructName(int field) const
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

void *HeadFlitDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    HeadFlit_Base *pp = (HeadFlit_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

AckFlit_Base::AckFlit_Base(const char *name, int kind) : ::RootFlit(name,kind)
{
    this->setSchedulingPriority(SP_EARLIER);
    this->setKind(ACK);
    this->setSequence_number(0);

    this->source = 0;
    this->destination = 0;
    this->generation_time = 0;
    this->injection_time = 0;
    this->packet_priority = 0;
    this->packet_id = 0;
    this->packet_attempts = 0;
    this->packet_num_flit = 0;
    this->packet_generation_time = 0;
    this->packet_injection_time = 0;
}

AckFlit_Base::AckFlit_Base(const AckFlit_Base& other) : ::RootFlit(other)
{
    copy(other);
}

AckFlit_Base::~AckFlit_Base()
{
}

AckFlit_Base& AckFlit_Base::operator=(const AckFlit_Base& other)
{
    if (this==&other) return *this;
    ::RootFlit::operator=(other);
    copy(other);
    return *this;
}

void AckFlit_Base::copy(const AckFlit_Base& other)
{
    this->source = other.source;
    this->destination = other.destination;
    this->generation_time = other.generation_time;
    this->injection_time = other.injection_time;
    this->packet_priority = other.packet_priority;
    this->packet_id = other.packet_id;
    this->packet_attempts = other.packet_attempts;
    this->packet_num_flit = other.packet_num_flit;
    this->packet_generation_time = other.packet_generation_time;
    this->packet_injection_time = other.packet_injection_time;
}

void AckFlit_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::RootFlit::parsimPack(b);
    doParsimPacking(b,this->source);
    doParsimPacking(b,this->destination);
    doParsimPacking(b,this->generation_time);
    doParsimPacking(b,this->injection_time);
    doParsimPacking(b,this->packet_priority);
    doParsimPacking(b,this->packet_id);
    doParsimPacking(b,this->packet_attempts);
    doParsimPacking(b,this->packet_num_flit);
    doParsimPacking(b,this->packet_generation_time);
    doParsimPacking(b,this->packet_injection_time);
}

void AckFlit_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::RootFlit::parsimUnpack(b);
    doParsimUnpacking(b,this->source);
    doParsimUnpacking(b,this->destination);
    doParsimUnpacking(b,this->generation_time);
    doParsimUnpacking(b,this->injection_time);
    doParsimUnpacking(b,this->packet_priority);
    doParsimUnpacking(b,this->packet_id);
    doParsimUnpacking(b,this->packet_attempts);
    doParsimUnpacking(b,this->packet_num_flit);
    doParsimUnpacking(b,this->packet_generation_time);
    doParsimUnpacking(b,this->packet_injection_time);
}

int AckFlit_Base::getSource() const
{
    return this->source;
}

void AckFlit_Base::setSource(int source)
{
    this->source = source;
}

int AckFlit_Base::getDestination() const
{
    return this->destination;
}

void AckFlit_Base::setDestination(int destination)
{
    this->destination = destination;
}

::omnetpp::simtime_t AckFlit_Base::getGeneration_time() const
{
    return this->generation_time;
}

void AckFlit_Base::setGeneration_time(::omnetpp::simtime_t generation_time)
{
    this->generation_time = generation_time;
}

::omnetpp::simtime_t AckFlit_Base::getInjection_time() const
{
    return this->injection_time;
}

void AckFlit_Base::setInjection_time(::omnetpp::simtime_t injection_time)
{
    this->injection_time = injection_time;
}

int AckFlit_Base::getPacket_priority() const
{
    return this->packet_priority;
}

void AckFlit_Base::setPacket_priority(int packet_priority)
{
    this->packet_priority = packet_priority;
}

long AckFlit_Base::getPacket_id() const
{
    return this->packet_id;
}

void AckFlit_Base::setPacket_id(long packet_id)
{
    this->packet_id = packet_id;
}

int AckFlit_Base::getPacket_attempts() const
{
    return this->packet_attempts;
}

void AckFlit_Base::setPacket_attempts(int packet_attempts)
{
    this->packet_attempts = packet_attempts;
}

int AckFlit_Base::getPacket_num_flit() const
{
    return this->packet_num_flit;
}

void AckFlit_Base::setPacket_num_flit(int packet_num_flit)
{
    this->packet_num_flit = packet_num_flit;
}

::omnetpp::simtime_t AckFlit_Base::getPacket_generation_time() const
{
    return this->packet_generation_time;
}

void AckFlit_Base::setPacket_generation_time(::omnetpp::simtime_t packet_generation_time)
{
    this->packet_generation_time = packet_generation_time;
}

::omnetpp::simtime_t AckFlit_Base::getPacket_injection_time() const
{
    return this->packet_injection_time;
}

void AckFlit_Base::setPacket_injection_time(::omnetpp::simtime_t packet_injection_time)
{
    this->packet_injection_time = packet_injection_time;
}

class AckFlitDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    AckFlitDescriptor();
    virtual ~AckFlitDescriptor();

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

Register_ClassDescriptor(AckFlitDescriptor);

AckFlitDescriptor::AckFlitDescriptor() : omnetpp::cClassDescriptor("AckFlit", "RootFlit")
{
    propertynames = nullptr;
}

AckFlitDescriptor::~AckFlitDescriptor()
{
    delete[] propertynames;
}

bool AckFlitDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<AckFlit_Base *>(obj)!=nullptr;
}

const char **AckFlitDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *AckFlitDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int AckFlitDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 10+basedesc->getFieldCount() : 10;
}

unsigned int AckFlitDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<10) ? fieldTypeFlags[field] : 0;
}

const char *AckFlitDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "source",
        "destination",
        "generation_time",
        "injection_time",
        "packet_priority",
        "packet_id",
        "packet_attempts",
        "packet_num_flit",
        "packet_generation_time",
        "packet_injection_time",
    };
    return (field>=0 && field<10) ? fieldNames[field] : nullptr;
}

int AckFlitDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "source")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destination")==0) return base+1;
    if (fieldName[0]=='g' && strcmp(fieldName, "generation_time")==0) return base+2;
    if (fieldName[0]=='i' && strcmp(fieldName, "injection_time")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_priority")==0) return base+4;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_id")==0) return base+5;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_attempts")==0) return base+6;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_num_flit")==0) return base+7;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_generation_time")==0) return base+8;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_injection_time")==0) return base+9;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *AckFlitDescriptor::getFieldTypeString(int field) const
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
        "simtime_t",
        "simtime_t",
        "int",
        "long",
        "int",
        "int",
        "simtime_t",
        "simtime_t",
    };
    return (field>=0 && field<10) ? fieldTypeStrings[field] : nullptr;
}

const char **AckFlitDescriptor::getFieldPropertyNames(int field) const
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

const char *AckFlitDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int AckFlitDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    AckFlit_Base *pp = (AckFlit_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string AckFlitDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    AckFlit_Base *pp = (AckFlit_Base *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSource());
        case 1: return long2string(pp->getDestination());
        case 2: return simtime2string(pp->getGeneration_time());
        case 3: return simtime2string(pp->getInjection_time());
        case 4: return long2string(pp->getPacket_priority());
        case 5: return long2string(pp->getPacket_id());
        case 6: return long2string(pp->getPacket_attempts());
        case 7: return long2string(pp->getPacket_num_flit());
        case 8: return simtime2string(pp->getPacket_generation_time());
        case 9: return simtime2string(pp->getPacket_injection_time());
        default: return "";
    }
}

bool AckFlitDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    AckFlit_Base *pp = (AckFlit_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setSource(string2long(value)); return true;
        case 1: pp->setDestination(string2long(value)); return true;
        case 2: pp->setGeneration_time(string2simtime(value)); return true;
        case 3: pp->setInjection_time(string2simtime(value)); return true;
        case 4: pp->setPacket_priority(string2long(value)); return true;
        case 5: pp->setPacket_id(string2long(value)); return true;
        case 6: pp->setPacket_attempts(string2long(value)); return true;
        case 7: pp->setPacket_num_flit(string2long(value)); return true;
        case 8: pp->setPacket_generation_time(string2simtime(value)); return true;
        case 9: pp->setPacket_injection_time(string2simtime(value)); return true;
        default: return false;
    }
}

const char *AckFlitDescriptor::getFieldStructName(int field) const
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

void *AckFlitDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    AckFlit_Base *pp = (AckFlit_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

NackFlit_Base::NackFlit_Base(const char *name, int kind) : ::RootFlit(name,kind)
{
    this->setSchedulingPriority(SP_EARLIER);
    this->setKind(NACK);
    this->setSequence_number(0);

    this->source = 0;
    this->destination = 0;
    this->generation_time = 0;
    this->injection_time = 0;
    this->packet_priority = 0;
    this->packet_id = 0;
    this->packet_attempts = 0;
    this->packet_num_flit = 0;
    this->packet_generation_time = 0;
    this->packet_injection_time = 0;
    this->carries_decoder_info = false;
}

NackFlit_Base::NackFlit_Base(const NackFlit_Base& other) : ::RootFlit(other)
{
    copy(other);
}

NackFlit_Base::~NackFlit_Base()
{
}

NackFlit_Base& NackFlit_Base::operator=(const NackFlit_Base& other)
{
    if (this==&other) return *this;
    ::RootFlit::operator=(other);
    copy(other);
    return *this;
}

void NackFlit_Base::copy(const NackFlit_Base& other)
{
    this->source = other.source;
    this->destination = other.destination;
    this->generation_time = other.generation_time;
    this->injection_time = other.injection_time;
    this->packet_priority = other.packet_priority;
    this->packet_id = other.packet_id;
    this->packet_attempts = other.packet_attempts;
    this->packet_num_flit = other.packet_num_flit;
    this->packet_generation_time = other.packet_generation_time;
    this->packet_injection_time = other.packet_injection_time;
    this->carries_decoder_info = other.carries_decoder_info;
    this->missing_flits = other.missing_flits;
}

void NackFlit_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::RootFlit::parsimPack(b);
    doParsimPacking(b,this->source);
    doParsimPacking(b,this->destination);
    doParsimPacking(b,this->generation_time);
    doParsimPacking(b,this->injection_time);
    doParsimPacking(b,this->packet_priority);
    doParsimPacking(b,this->packet_id);
    doParsimPacking(b,this->packet_attempts);
    doParsimPacking(b,this->packet_num_flit);
    doParsimPacking(b,this->packet_generation_time);
    doParsimPacking(b,this->packet_injection_time);
    doParsimPacking(b,this->carries_decoder_info);
    doParsimPacking(b,this->missing_flits);
}

void NackFlit_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::RootFlit::parsimUnpack(b);
    doParsimUnpacking(b,this->source);
    doParsimUnpacking(b,this->destination);
    doParsimUnpacking(b,this->generation_time);
    doParsimUnpacking(b,this->injection_time);
    doParsimUnpacking(b,this->packet_priority);
    doParsimUnpacking(b,this->packet_id);
    doParsimUnpacking(b,this->packet_attempts);
    doParsimUnpacking(b,this->packet_num_flit);
    doParsimUnpacking(b,this->packet_generation_time);
    doParsimUnpacking(b,this->packet_injection_time);
    doParsimUnpacking(b,this->carries_decoder_info);
    doParsimUnpacking(b,this->missing_flits);
}

int NackFlit_Base::getSource() const
{
    return this->source;
}

void NackFlit_Base::setSource(int source)
{
    this->source = source;
}

int NackFlit_Base::getDestination() const
{
    return this->destination;
}

void NackFlit_Base::setDestination(int destination)
{
    this->destination = destination;
}

::omnetpp::simtime_t NackFlit_Base::getGeneration_time() const
{
    return this->generation_time;
}

void NackFlit_Base::setGeneration_time(::omnetpp::simtime_t generation_time)
{
    this->generation_time = generation_time;
}

::omnetpp::simtime_t NackFlit_Base::getInjection_time() const
{
    return this->injection_time;
}

void NackFlit_Base::setInjection_time(::omnetpp::simtime_t injection_time)
{
    this->injection_time = injection_time;
}

int NackFlit_Base::getPacket_priority() const
{
    return this->packet_priority;
}

void NackFlit_Base::setPacket_priority(int packet_priority)
{
    this->packet_priority = packet_priority;
}

long NackFlit_Base::getPacket_id() const
{
    return this->packet_id;
}

void NackFlit_Base::setPacket_id(long packet_id)
{
    this->packet_id = packet_id;
}

int NackFlit_Base::getPacket_attempts() const
{
    return this->packet_attempts;
}

void NackFlit_Base::setPacket_attempts(int packet_attempts)
{
    this->packet_attempts = packet_attempts;
}

int NackFlit_Base::getPacket_num_flit() const
{
    return this->packet_num_flit;
}

void NackFlit_Base::setPacket_num_flit(int packet_num_flit)
{
    this->packet_num_flit = packet_num_flit;
}

::omnetpp::simtime_t NackFlit_Base::getPacket_generation_time() const
{
    return this->packet_generation_time;
}

void NackFlit_Base::setPacket_generation_time(::omnetpp::simtime_t packet_generation_time)
{
    this->packet_generation_time = packet_generation_time;
}

::omnetpp::simtime_t NackFlit_Base::getPacket_injection_time() const
{
    return this->packet_injection_time;
}

void NackFlit_Base::setPacket_injection_time(::omnetpp::simtime_t packet_injection_time)
{
    this->packet_injection_time = packet_injection_time;
}

bool NackFlit_Base::getCarries_decoder_info() const
{
    return this->carries_decoder_info;
}

void NackFlit_Base::setCarries_decoder_info(bool carries_decoder_info)
{
    this->carries_decoder_info = carries_decoder_info;
}

IntVector& NackFlit_Base::getMissing_flits()
{
    return this->missing_flits;
}

void NackFlit_Base::setMissing_flits(const IntVector& missing_flits)
{
    this->missing_flits = missing_flits;
}

class NackFlitDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    NackFlitDescriptor();
    virtual ~NackFlitDescriptor();

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

Register_ClassDescriptor(NackFlitDescriptor);

NackFlitDescriptor::NackFlitDescriptor() : omnetpp::cClassDescriptor("NackFlit", "RootFlit")
{
    propertynames = nullptr;
}

NackFlitDescriptor::~NackFlitDescriptor()
{
    delete[] propertynames;
}

bool NackFlitDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<NackFlit_Base *>(obj)!=nullptr;
}

const char **NackFlitDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *NackFlitDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int NackFlitDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 12+basedesc->getFieldCount() : 12;
}

unsigned int NackFlitDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<12) ? fieldTypeFlags[field] : 0;
}

const char *NackFlitDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "source",
        "destination",
        "generation_time",
        "injection_time",
        "packet_priority",
        "packet_id",
        "packet_attempts",
        "packet_num_flit",
        "packet_generation_time",
        "packet_injection_time",
        "carries_decoder_info",
        "missing_flits",
    };
    return (field>=0 && field<12) ? fieldNames[field] : nullptr;
}

int NackFlitDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "source")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destination")==0) return base+1;
    if (fieldName[0]=='g' && strcmp(fieldName, "generation_time")==0) return base+2;
    if (fieldName[0]=='i' && strcmp(fieldName, "injection_time")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_priority")==0) return base+4;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_id")==0) return base+5;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_attempts")==0) return base+6;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_num_flit")==0) return base+7;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_generation_time")==0) return base+8;
    if (fieldName[0]=='p' && strcmp(fieldName, "packet_injection_time")==0) return base+9;
    if (fieldName[0]=='c' && strcmp(fieldName, "carries_decoder_info")==0) return base+10;
    if (fieldName[0]=='m' && strcmp(fieldName, "missing_flits")==0) return base+11;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *NackFlitDescriptor::getFieldTypeString(int field) const
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
        "simtime_t",
        "simtime_t",
        "int",
        "long",
        "int",
        "int",
        "simtime_t",
        "simtime_t",
        "bool",
        "IntVector",
    };
    return (field>=0 && field<12) ? fieldTypeStrings[field] : nullptr;
}

const char **NackFlitDescriptor::getFieldPropertyNames(int field) const
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

const char *NackFlitDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int NackFlitDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    NackFlit_Base *pp = (NackFlit_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string NackFlitDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    NackFlit_Base *pp = (NackFlit_Base *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSource());
        case 1: return long2string(pp->getDestination());
        case 2: return simtime2string(pp->getGeneration_time());
        case 3: return simtime2string(pp->getInjection_time());
        case 4: return long2string(pp->getPacket_priority());
        case 5: return long2string(pp->getPacket_id());
        case 6: return long2string(pp->getPacket_attempts());
        case 7: return long2string(pp->getPacket_num_flit());
        case 8: return simtime2string(pp->getPacket_generation_time());
        case 9: return simtime2string(pp->getPacket_injection_time());
        case 10: return bool2string(pp->getCarries_decoder_info());
        case 11: {std::stringstream out; out << pp->getMissing_flits(); return out.str();}
        default: return "";
    }
}

bool NackFlitDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    NackFlit_Base *pp = (NackFlit_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setSource(string2long(value)); return true;
        case 1: pp->setDestination(string2long(value)); return true;
        case 2: pp->setGeneration_time(string2simtime(value)); return true;
        case 3: pp->setInjection_time(string2simtime(value)); return true;
        case 4: pp->setPacket_priority(string2long(value)); return true;
        case 5: pp->setPacket_id(string2long(value)); return true;
        case 6: pp->setPacket_attempts(string2long(value)); return true;
        case 7: pp->setPacket_num_flit(string2long(value)); return true;
        case 8: pp->setPacket_generation_time(string2simtime(value)); return true;
        case 9: pp->setPacket_injection_time(string2simtime(value)); return true;
        case 10: pp->setCarries_decoder_info(string2bool(value)); return true;
        default: return false;
    }
}

const char *NackFlitDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 11: return omnetpp::opp_typename(typeid(IntVector));
        default: return nullptr;
    };
}

void *NackFlitDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    NackFlit_Base *pp = (NackFlit_Base *)object; (void)pp;
    switch (field) {
        case 11: return (void *)(&pp->getMissing_flits()); break;
        default: return nullptr;
    }
}

CNackFlit_Base::CNackFlit_Base(const char *name, int kind) : ::RootFlit(name,kind)
{
    this->setSchedulingPriority(SP_EARLIER);
    this->setKind(CNACK);
    this->setSequence_number(0);

    this->num_nacks_combined = 0;
}

CNackFlit_Base::CNackFlit_Base(const CNackFlit_Base& other) : ::RootFlit(other)
{
    copy(other);
}

CNackFlit_Base::~CNackFlit_Base()
{
}

CNackFlit_Base& CNackFlit_Base::operator=(const CNackFlit_Base& other)
{
    if (this==&other) return *this;
    ::RootFlit::operator=(other);
    copy(other);
    return *this;
}

void CNackFlit_Base::copy(const CNackFlit_Base& other)
{
    this->num_nacks_combined = other.num_nacks_combined;
}

void CNackFlit_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::RootFlit::parsimPack(b);
    doParsimPacking(b,this->num_nacks_combined);
}

void CNackFlit_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::RootFlit::parsimUnpack(b);
    doParsimUnpacking(b,this->num_nacks_combined);
}

int CNackFlit_Base::getNum_nacks_combined() const
{
    return this->num_nacks_combined;
}

void CNackFlit_Base::setNum_nacks_combined(int num_nacks_combined)
{
    this->num_nacks_combined = num_nacks_combined;
}

class CNackFlitDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    CNackFlitDescriptor();
    virtual ~CNackFlitDescriptor();

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

Register_ClassDescriptor(CNackFlitDescriptor);

CNackFlitDescriptor::CNackFlitDescriptor() : omnetpp::cClassDescriptor("CNackFlit", "RootFlit")
{
    propertynames = nullptr;
}

CNackFlitDescriptor::~CNackFlitDescriptor()
{
    delete[] propertynames;
}

bool CNackFlitDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CNackFlit_Base *>(obj)!=nullptr;
}

const char **CNackFlitDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *CNackFlitDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname,"customize")) return "true";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int CNackFlitDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount() : 1;
}

unsigned int CNackFlitDescriptor::getFieldTypeFlags(int field) const
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

const char *CNackFlitDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "num_nacks_combined",
    };
    return (field>=0 && field<1) ? fieldNames[field] : nullptr;
}

int CNackFlitDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "num_nacks_combined")==0) return base+0;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *CNackFlitDescriptor::getFieldTypeString(int field) const
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

const char **CNackFlitDescriptor::getFieldPropertyNames(int field) const
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

const char *CNackFlitDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int CNackFlitDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    CNackFlit_Base *pp = (CNackFlit_Base *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string CNackFlitDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    CNackFlit_Base *pp = (CNackFlit_Base *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getNum_nacks_combined());
        default: return "";
    }
}

bool CNackFlitDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    CNackFlit_Base *pp = (CNackFlit_Base *)object; (void)pp;
    switch (field) {
        case 0: pp->setNum_nacks_combined(string2long(value)); return true;
        default: return false;
    }
}

const char *CNackFlitDescriptor::getFieldStructName(int field) const
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

void *CNackFlitDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    CNackFlit_Base *pp = (CNackFlit_Base *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


