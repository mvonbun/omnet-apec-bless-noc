//
// Generated file, do not edit! Created by nedtool 5.0 from utilities/enum_definitions.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "enum_definitions_m.h"

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

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("MessageTypes");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("MessageTypes"));
    e->insert(EVENT, "EVENT");
    e->insert(TXEVENT, "TXEVENT");
    e->insert(TIMEOUT, "TIMEOUT");
    e->insert(ARBEVENT, "ARBEVENT");
    e->insert(RELEVENT, "RELEVENT");
    e->insert(PACKTRIG, "PACKTRIG");
    e->insert(MONITOR, "MONITOR");
    e->insert(FLIT, "FLIT");
    e->insert(HEAD, "HEAD");
    e->insert(HEADTAIL, "HEADTAIL");
    e->insert(ACK, "ACK");
    e->insert(NACK, "NACK");
    e->insert(CNACK, "CNACK");
    e->insert(DATA, "DATA");
    e->insert(BODY, "BODY");
    e->insert(TAIL, "TAIL");
    e->insert(CTRL, "CTRL");
    e->insert(CBUFOVFL, "CBUFOVFL");
    e->insert(CCONV, "CCONV");
);

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("SchedulingPriorities");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("SchedulingPriorities"));
    e->insert(SP_FIRST, "SP_FIRST");
    e->insert(SP_VERY_EARLY, "SP_VERY_EARLY");
    e->insert(SP_EARLY, "SP_EARLY");
    e->insert(SP_EARLIER, "SP_EARLIER");
    e->insert(SP_BASE, "SP_BASE");
    e->insert(SP_LATER, "SP_LATER");
    e->insert(SP_LATE, "SP_LATE");
    e->insert(SP_VERY_LATE, "SP_VERY_LATE");
    e->insert(SP_LAST, "SP_LAST");
);

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("PacketPriorities");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("PacketPriorities"));
    e->insert(PRIO_PACKET_HIGHEST, "PRIO_PACKET_HIGHEST");
    e->insert(PRIO_PACKET_VERYHIGH, "PRIO_PACKET_VERYHIGH");
    e->insert(PRIO_PACKET_HIGHER, "PRIO_PACKET_HIGHER");
    e->insert(PRIO_PACKET_HIGH, "PRIO_PACKET_HIGH");
    e->insert(PRIO_PACKET_BASE, "PRIO_PACKET_BASE");
    e->insert(PRIO_PACKET_LOW, "PRIO_PACKET_LOW");
    e->insert(PRIO_PACKET_LOWER, "PRIO_PACKET_LOWER");
    e->insert(PRIO_PACKET_VERYLOW, "PRIO_PACKET_VERYLOW");
    e->insert(PRIO_PACKET_LOWEST, "PRIO_PACKET_LOWEST");
);

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("CodeTypes");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("CodeTypes"));
    e->insert(UNCODED, "UNCODED");
    e->insert(CYCLIC, "CYCLIC");
    e->insert(DYNAMIC, "DYNAMIC");
    e->insert(HEUR, "HEUR");
    e->insert(HEUR_S, "HEUR_S");
    e->insert(HEUR_B, "HEUR_B");
    e->insert(HEUR_J, "HEUR_J");
    e->insert(FEC, "FEC");
    e->insert(BLOCKX, "BLOCKX");
);

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("ResourceReleaseTypes");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("ResourceReleaseTypes"));
    e->insert(TAILFLAG, "TAILFLAG");
    e->insert(FLITCOUNT, "FLITCOUNT");
);

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("ModuleTypes");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("ModuleTypes"));
    e->insert(SOURCE, "SOURCE");
    e->insert(ENCODER, "ENCODER");
    e->insert(NI, "NI");
    e->insert(ROUTER, "ROUTER");
    e->insert(DECODER, "DECODER");
    e->insert(SINK, "SINK");
);


