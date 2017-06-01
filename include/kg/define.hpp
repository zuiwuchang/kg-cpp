#ifndef KG_DEFINE_HEADER_HPP

//typedef name to type_t
#define KG_TYPEDEF_T(name)	typedef name type_t;
//typedef boost::shared_ptr<name> to type_spt
#define KG_TYPEDEF_SPT(name)	typedef boost::shared_ptr<name> type_spt;

//KG_TYPEDEF_T and KG_TYPEDEF_SPT
#define KG_TYPEDEF_TT(name) KG_TYPEDEF_T(name) KG_TYPEDEF_SPT(name)

#endif // KG_DEFINE_HEADER_HPP
