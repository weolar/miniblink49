

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for msxml.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __msxml_h__
#define __msxml_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IXMLDOMImplementation_FWD_DEFINED__
#define __IXMLDOMImplementation_FWD_DEFINED__
typedef interface IXMLDOMImplementation IXMLDOMImplementation;
#endif 	/* __IXMLDOMImplementation_FWD_DEFINED__ */


#ifndef __IXMLDOMNode_FWD_DEFINED__
#define __IXMLDOMNode_FWD_DEFINED__
typedef interface IXMLDOMNode IXMLDOMNode;
#endif 	/* __IXMLDOMNode_FWD_DEFINED__ */


#ifndef __IXMLDOMDocumentFragment_FWD_DEFINED__
#define __IXMLDOMDocumentFragment_FWD_DEFINED__
typedef interface IXMLDOMDocumentFragment IXMLDOMDocumentFragment;
#endif 	/* __IXMLDOMDocumentFragment_FWD_DEFINED__ */


#ifndef __IXMLDOMDocument_FWD_DEFINED__
#define __IXMLDOMDocument_FWD_DEFINED__
typedef interface IXMLDOMDocument IXMLDOMDocument;
#endif 	/* __IXMLDOMDocument_FWD_DEFINED__ */


#ifndef __IXMLDOMNodeList_FWD_DEFINED__
#define __IXMLDOMNodeList_FWD_DEFINED__
typedef interface IXMLDOMNodeList IXMLDOMNodeList;
#endif 	/* __IXMLDOMNodeList_FWD_DEFINED__ */


#ifndef __IXMLDOMNamedNodeMap_FWD_DEFINED__
#define __IXMLDOMNamedNodeMap_FWD_DEFINED__
typedef interface IXMLDOMNamedNodeMap IXMLDOMNamedNodeMap;
#endif 	/* __IXMLDOMNamedNodeMap_FWD_DEFINED__ */


#ifndef __IXMLDOMCharacterData_FWD_DEFINED__
#define __IXMLDOMCharacterData_FWD_DEFINED__
typedef interface IXMLDOMCharacterData IXMLDOMCharacterData;
#endif 	/* __IXMLDOMCharacterData_FWD_DEFINED__ */


#ifndef __IXMLDOMAttribute_FWD_DEFINED__
#define __IXMLDOMAttribute_FWD_DEFINED__
typedef interface IXMLDOMAttribute IXMLDOMAttribute;
#endif 	/* __IXMLDOMAttribute_FWD_DEFINED__ */


#ifndef __IXMLDOMElement_FWD_DEFINED__
#define __IXMLDOMElement_FWD_DEFINED__
typedef interface IXMLDOMElement IXMLDOMElement;
#endif 	/* __IXMLDOMElement_FWD_DEFINED__ */


#ifndef __IXMLDOMText_FWD_DEFINED__
#define __IXMLDOMText_FWD_DEFINED__
typedef interface IXMLDOMText IXMLDOMText;
#endif 	/* __IXMLDOMText_FWD_DEFINED__ */


#ifndef __IXMLDOMComment_FWD_DEFINED__
#define __IXMLDOMComment_FWD_DEFINED__
typedef interface IXMLDOMComment IXMLDOMComment;
#endif 	/* __IXMLDOMComment_FWD_DEFINED__ */


#ifndef __IXMLDOMProcessingInstruction_FWD_DEFINED__
#define __IXMLDOMProcessingInstruction_FWD_DEFINED__
typedef interface IXMLDOMProcessingInstruction IXMLDOMProcessingInstruction;
#endif 	/* __IXMLDOMProcessingInstruction_FWD_DEFINED__ */


#ifndef __IXMLDOMCDATASection_FWD_DEFINED__
#define __IXMLDOMCDATASection_FWD_DEFINED__
typedef interface IXMLDOMCDATASection IXMLDOMCDATASection;
#endif 	/* __IXMLDOMCDATASection_FWD_DEFINED__ */


#ifndef __IXMLDOMDocumentType_FWD_DEFINED__
#define __IXMLDOMDocumentType_FWD_DEFINED__
typedef interface IXMLDOMDocumentType IXMLDOMDocumentType;
#endif 	/* __IXMLDOMDocumentType_FWD_DEFINED__ */


#ifndef __IXMLDOMNotation_FWD_DEFINED__
#define __IXMLDOMNotation_FWD_DEFINED__
typedef interface IXMLDOMNotation IXMLDOMNotation;
#endif 	/* __IXMLDOMNotation_FWD_DEFINED__ */


#ifndef __IXMLDOMEntity_FWD_DEFINED__
#define __IXMLDOMEntity_FWD_DEFINED__
typedef interface IXMLDOMEntity IXMLDOMEntity;
#endif 	/* __IXMLDOMEntity_FWD_DEFINED__ */


#ifndef __IXMLDOMEntityReference_FWD_DEFINED__
#define __IXMLDOMEntityReference_FWD_DEFINED__
typedef interface IXMLDOMEntityReference IXMLDOMEntityReference;
#endif 	/* __IXMLDOMEntityReference_FWD_DEFINED__ */


#ifndef __IXMLDOMParseError_FWD_DEFINED__
#define __IXMLDOMParseError_FWD_DEFINED__
typedef interface IXMLDOMParseError IXMLDOMParseError;
#endif 	/* __IXMLDOMParseError_FWD_DEFINED__ */


#ifndef __IXTLRuntime_FWD_DEFINED__
#define __IXTLRuntime_FWD_DEFINED__
typedef interface IXTLRuntime IXTLRuntime;
#endif 	/* __IXTLRuntime_FWD_DEFINED__ */


#ifndef __XMLDOMDocumentEvents_FWD_DEFINED__
#define __XMLDOMDocumentEvents_FWD_DEFINED__
typedef interface XMLDOMDocumentEvents XMLDOMDocumentEvents;
#endif 	/* __XMLDOMDocumentEvents_FWD_DEFINED__ */


#ifndef __DOMDocument_FWD_DEFINED__
#define __DOMDocument_FWD_DEFINED__

#ifdef __cplusplus
typedef class DOMDocument DOMDocument;
#else
typedef struct DOMDocument DOMDocument;
#endif /* __cplusplus */

#endif 	/* __DOMDocument_FWD_DEFINED__ */


#ifndef __DOMFreeThreadedDocument_FWD_DEFINED__
#define __DOMFreeThreadedDocument_FWD_DEFINED__

#ifdef __cplusplus
typedef class DOMFreeThreadedDocument DOMFreeThreadedDocument;
#else
typedef struct DOMFreeThreadedDocument DOMFreeThreadedDocument;
#endif /* __cplusplus */

#endif 	/* __DOMFreeThreadedDocument_FWD_DEFINED__ */


#ifndef __IXMLHttpRequest_FWD_DEFINED__
#define __IXMLHttpRequest_FWD_DEFINED__
typedef interface IXMLHttpRequest IXMLHttpRequest;
#endif 	/* __IXMLHttpRequest_FWD_DEFINED__ */


#ifndef __XMLHTTPRequest_FWD_DEFINED__
#define __XMLHTTPRequest_FWD_DEFINED__

#ifdef __cplusplus
typedef class XMLHTTPRequest XMLHTTPRequest;
#else
typedef struct XMLHTTPRequest XMLHTTPRequest;
#endif /* __cplusplus */

#endif 	/* __XMLHTTPRequest_FWD_DEFINED__ */


#ifndef __IXMLDSOControl_FWD_DEFINED__
#define __IXMLDSOControl_FWD_DEFINED__
typedef interface IXMLDSOControl IXMLDSOControl;
#endif 	/* __IXMLDSOControl_FWD_DEFINED__ */


#ifndef __XMLDSOControl_FWD_DEFINED__
#define __XMLDSOControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class XMLDSOControl XMLDSOControl;
#else
typedef struct XMLDSOControl XMLDSOControl;
#endif /* __cplusplus */

#endif 	/* __XMLDSOControl_FWD_DEFINED__ */


#ifndef __IXMLElementCollection_FWD_DEFINED__
#define __IXMLElementCollection_FWD_DEFINED__
typedef interface IXMLElementCollection IXMLElementCollection;
#endif 	/* __IXMLElementCollection_FWD_DEFINED__ */


#ifndef __IXMLDocument_FWD_DEFINED__
#define __IXMLDocument_FWD_DEFINED__
typedef interface IXMLDocument IXMLDocument;
#endif 	/* __IXMLDocument_FWD_DEFINED__ */


#ifndef __IXMLDocument2_FWD_DEFINED__
#define __IXMLDocument2_FWD_DEFINED__
typedef interface IXMLDocument2 IXMLDocument2;
#endif 	/* __IXMLDocument2_FWD_DEFINED__ */


#ifndef __IXMLElement_FWD_DEFINED__
#define __IXMLElement_FWD_DEFINED__
typedef interface IXMLElement IXMLElement;
#endif 	/* __IXMLElement_FWD_DEFINED__ */


#ifndef __IXMLElement2_FWD_DEFINED__
#define __IXMLElement2_FWD_DEFINED__
typedef interface IXMLElement2 IXMLElement2;
#endif 	/* __IXMLElement2_FWD_DEFINED__ */


#ifndef __IXMLAttribute_FWD_DEFINED__
#define __IXMLAttribute_FWD_DEFINED__
typedef interface IXMLAttribute IXMLAttribute;
#endif 	/* __IXMLAttribute_FWD_DEFINED__ */


#ifndef __IXMLError_FWD_DEFINED__
#define __IXMLError_FWD_DEFINED__
typedef interface IXMLError IXMLError;
#endif 	/* __IXMLError_FWD_DEFINED__ */


#ifndef __XMLDocument_FWD_DEFINED__
#define __XMLDocument_FWD_DEFINED__

#ifdef __cplusplus
typedef class XMLDocument XMLDocument;
#else
typedef struct XMLDocument XMLDocument;
#endif /* __cplusplus */

#endif 	/* __XMLDocument_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "objidl.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_msxml_0000 */
/* [local] */ 

//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997-1998.
//
//--------------------------------------------------------------------------
typedef struct _xml_error
    {
    unsigned int _nLine;
    BSTR _pchBuf;
    unsigned int _cchBuf;
    unsigned int _ich;
    BSTR _pszFound;
    BSTR _pszExpected;
    DWORD _reserved1;
    DWORD _reserved2;
    } 	XML_ERROR;



extern RPC_IF_HANDLE __MIDL_itf_msxml_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_msxml_0000_v0_0_s_ifspec;


#ifndef __MSXML_LIBRARY_DEFINED__
#define __MSXML_LIBRARY_DEFINED__

/* library MSXML */
/* [version][lcid][helpstring][uuid] */ 

//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1998.
//
//--------------------------------------------------------------------------



















typedef /* [helpstring] */ 
enum tagDOMNodeType
    {	NODE_INVALID	= 0,
	NODE_ELEMENT	= NODE_INVALID + 1,
	NODE_ATTRIBUTE	= NODE_ELEMENT + 1,
	NODE_TEXT	= NODE_ATTRIBUTE + 1,
	NODE_CDATA_SECTION	= NODE_TEXT + 1,
	NODE_ENTITY_REFERENCE	= NODE_CDATA_SECTION + 1,
	NODE_ENTITY	= NODE_ENTITY_REFERENCE + 1,
	NODE_PROCESSING_INSTRUCTION	= NODE_ENTITY + 1,
	NODE_COMMENT	= NODE_PROCESSING_INSTRUCTION + 1,
	NODE_DOCUMENT	= NODE_COMMENT + 1,
	NODE_DOCUMENT_TYPE	= NODE_DOCUMENT + 1,
	NODE_DOCUMENT_FRAGMENT	= NODE_DOCUMENT_TYPE + 1,
	NODE_NOTATION	= NODE_DOCUMENT_FRAGMENT + 1
    } 	DOMNodeType;

//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1998.
//
//--------------------------------------------------------------------------









typedef /* [helpstring] */ 
enum tagXMLEMEM_TYPE
    {	XMLELEMTYPE_ELEMENT	= 0,
	XMLELEMTYPE_TEXT	= XMLELEMTYPE_ELEMENT + 1,
	XMLELEMTYPE_COMMENT	= XMLELEMTYPE_TEXT + 1,
	XMLELEMTYPE_DOCUMENT	= XMLELEMTYPE_COMMENT + 1,
	XMLELEMTYPE_DTD	= XMLELEMTYPE_DOCUMENT + 1,
	XMLELEMTYPE_PI	= XMLELEMTYPE_DTD + 1,
	XMLELEMTYPE_OTHER	= XMLELEMTYPE_PI + 1
    } 	XMLELEM_TYPE;


EXTERN_C const IID LIBID_MSXML;

#ifndef __IXMLDOMImplementation_INTERFACE_DEFINED__
#define __IXMLDOMImplementation_INTERFACE_DEFINED__

/* interface IXMLDOMImplementation */
/* [uuid][dual][oleautomation][unique][nonextensible][object][local] */ 


EXTERN_C const IID IID_IXMLDOMImplementation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF8F-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMImplementation : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE hasFeature( 
            /* [in] */ BSTR feature,
            /* [in] */ BSTR version,
            /* [retval][out] */ VARIANT_BOOL *hasFeature) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMImplementationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMImplementation * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMImplementation * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMImplementation * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMImplementation * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMImplementation * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMImplementation * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMImplementation * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *hasFeature )( 
            IXMLDOMImplementation * This,
            /* [in] */ BSTR feature,
            /* [in] */ BSTR version,
            /* [retval][out] */ VARIANT_BOOL *hasFeature);
        
        END_INTERFACE
    } IXMLDOMImplementationVtbl;

    interface IXMLDOMImplementation
    {
        CONST_VTBL struct IXMLDOMImplementationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMImplementation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMImplementation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMImplementation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMImplementation_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMImplementation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMImplementation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMImplementation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMImplementation_hasFeature(This,feature,version,hasFeature)	\
    (This)->lpVtbl -> hasFeature(This,feature,version,hasFeature)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IXMLDOMImplementation_hasFeature_Proxy( 
    IXMLDOMImplementation * This,
    /* [in] */ BSTR feature,
    /* [in] */ BSTR version,
    /* [retval][out] */ VARIANT_BOOL *hasFeature);


void __RPC_STUB IXMLDOMImplementation_hasFeature_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMImplementation_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMNode_INTERFACE_DEFINED__
#define __IXMLDOMNode_INTERFACE_DEFINED__

/* interface IXMLDOMNode */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMNode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF80-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMNode : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_nodeName( 
            /* [retval][out] */ BSTR *name) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_nodeValue( 
            /* [retval][out] */ VARIANT *value) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_nodeValue( 
            /* [in] */ VARIANT value) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_nodeType( 
            /* [retval][out] */ DOMNodeType *type) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_parentNode( 
            /* [retval][out] */ IXMLDOMNode **parent) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_childNodes( 
            /* [retval][out] */ IXMLDOMNodeList **childList) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_firstChild( 
            /* [retval][out] */ IXMLDOMNode **firstChild) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_lastChild( 
            /* [retval][out] */ IXMLDOMNode **lastChild) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_previousSibling( 
            /* [retval][out] */ IXMLDOMNode **previousSibling) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_nextSibling( 
            /* [retval][out] */ IXMLDOMNode **nextSibling) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_attributes( 
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE insertBefore( 
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE replaceChild( 
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE removeChild( 
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE appendChild( 
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE hasChildNodes( 
            /* [retval][out] */ VARIANT_BOOL *hasChild) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ownerDocument( 
            /* [retval][out] */ IXMLDOMDocument **DOMDocument) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE cloneNode( 
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_nodeTypeString( 
            /* [out][retval] */ BSTR *nodeType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_text( 
            /* [out][retval] */ BSTR *text) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_text( 
            /* [in] */ BSTR text) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_specified( 
            /* [retval][out] */ VARIANT_BOOL *isSpecified) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_definition( 
            /* [out][retval] */ IXMLDOMNode **definitionNode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_nodeTypedValue( 
            /* [out][retval] */ VARIANT *typedValue) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_nodeTypedValue( 
            /* [in] */ VARIANT typedValue) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_dataType( 
            /* [out][retval] */ VARIANT *dataTypeName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_dataType( 
            /* [in] */ BSTR dataTypeName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_xml( 
            /* [out][retval] */ BSTR *xmlString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE transformNode( 
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE selectNodes( 
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE selectSingleNode( 
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_parsed( 
            /* [out][retval] */ VARIANT_BOOL *isParsed) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_namespaceURI( 
            /* [out][retval] */ BSTR *namespaceURI) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_prefix( 
            /* [out][retval] */ BSTR *prefixString) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_baseName( 
            /* [out][retval] */ BSTR *nameString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE transformNodeToObject( 
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMNodeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMNode * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMNode * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMNode * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMNode * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMNode * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMNode * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMNode * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMNode * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMNode * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMNode * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMNode * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMNode * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMNode * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMNode * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMNode * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMNode * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMNode * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMNode * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMNode * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMNode * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMNode * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMNode * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMNode * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMNode * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMNode * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMNode * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMNode * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMNode * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMNode * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMNode * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMNode * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMNode * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMNode * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMNode * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMNode * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMNode * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMNode * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMNode * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMNode * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMNode * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMNode * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMNode * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMNode * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        END_INTERFACE
    } IXMLDOMNodeVtbl;

    interface IXMLDOMNode
    {
        CONST_VTBL struct IXMLDOMNodeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMNode_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMNode_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMNode_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMNode_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMNode_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMNode_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMNode_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMNode_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMNode_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMNode_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMNode_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMNode_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMNode_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMNode_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMNode_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMNode_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMNode_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMNode_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMNode_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMNode_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMNode_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMNode_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMNode_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMNode_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMNode_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMNode_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMNode_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMNode_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMNode_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMNode_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMNode_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMNode_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMNode_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMNode_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMNode_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMNode_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMNode_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMNode_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMNode_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMNode_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMNode_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMNode_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMNode_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_nodeName_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ BSTR *name);


void __RPC_STUB IXMLDOMNode_get_nodeName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_nodeValue_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ VARIANT *value);


void __RPC_STUB IXMLDOMNode_get_nodeValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_put_nodeValue_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ VARIANT value);


void __RPC_STUB IXMLDOMNode_put_nodeValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_nodeType_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ DOMNodeType *type);


void __RPC_STUB IXMLDOMNode_get_nodeType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_parentNode_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ IXMLDOMNode **parent);


void __RPC_STUB IXMLDOMNode_get_parentNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_childNodes_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ IXMLDOMNodeList **childList);


void __RPC_STUB IXMLDOMNode_get_childNodes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_firstChild_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ IXMLDOMNode **firstChild);


void __RPC_STUB IXMLDOMNode_get_firstChild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_lastChild_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ IXMLDOMNode **lastChild);


void __RPC_STUB IXMLDOMNode_get_lastChild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_previousSibling_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ IXMLDOMNode **previousSibling);


void __RPC_STUB IXMLDOMNode_get_previousSibling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_nextSibling_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ IXMLDOMNode **nextSibling);


void __RPC_STUB IXMLDOMNode_get_nextSibling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_attributes_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);


void __RPC_STUB IXMLDOMNode_get_attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_insertBefore_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ IXMLDOMNode *newChild,
    /* [in] */ VARIANT refChild,
    /* [retval][out] */ IXMLDOMNode **outNewChild);


void __RPC_STUB IXMLDOMNode_insertBefore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_replaceChild_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ IXMLDOMNode *newChild,
    /* [in] */ IXMLDOMNode *oldChild,
    /* [retval][out] */ IXMLDOMNode **outOldChild);


void __RPC_STUB IXMLDOMNode_replaceChild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_removeChild_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ IXMLDOMNode *childNode,
    /* [retval][out] */ IXMLDOMNode **oldChild);


void __RPC_STUB IXMLDOMNode_removeChild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_appendChild_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ IXMLDOMNode *newChild,
    /* [retval][out] */ IXMLDOMNode **outNewChild);


void __RPC_STUB IXMLDOMNode_appendChild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_hasChildNodes_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ VARIANT_BOOL *hasChild);


void __RPC_STUB IXMLDOMNode_hasChildNodes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_ownerDocument_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ IXMLDOMDocument **DOMDocument);


void __RPC_STUB IXMLDOMNode_get_ownerDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_cloneNode_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ VARIANT_BOOL deep,
    /* [retval][out] */ IXMLDOMNode **cloneRoot);


void __RPC_STUB IXMLDOMNode_cloneNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_nodeTypeString_Proxy( 
    IXMLDOMNode * This,
    /* [out][retval] */ BSTR *nodeType);


void __RPC_STUB IXMLDOMNode_get_nodeTypeString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_text_Proxy( 
    IXMLDOMNode * This,
    /* [out][retval] */ BSTR *text);


void __RPC_STUB IXMLDOMNode_get_text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_put_text_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ BSTR text);


void __RPC_STUB IXMLDOMNode_put_text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_specified_Proxy( 
    IXMLDOMNode * This,
    /* [retval][out] */ VARIANT_BOOL *isSpecified);


void __RPC_STUB IXMLDOMNode_get_specified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_definition_Proxy( 
    IXMLDOMNode * This,
    /* [out][retval] */ IXMLDOMNode **definitionNode);


void __RPC_STUB IXMLDOMNode_get_definition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_nodeTypedValue_Proxy( 
    IXMLDOMNode * This,
    /* [out][retval] */ VARIANT *typedValue);


void __RPC_STUB IXMLDOMNode_get_nodeTypedValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_put_nodeTypedValue_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ VARIANT typedValue);


void __RPC_STUB IXMLDOMNode_put_nodeTypedValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_dataType_Proxy( 
    IXMLDOMNode * This,
    /* [out][retval] */ VARIANT *dataTypeName);


void __RPC_STUB IXMLDOMNode_get_dataType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_put_dataType_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ BSTR dataTypeName);


void __RPC_STUB IXMLDOMNode_put_dataType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_xml_Proxy( 
    IXMLDOMNode * This,
    /* [out][retval] */ BSTR *xmlString);


void __RPC_STUB IXMLDOMNode_get_xml_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_transformNode_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ IXMLDOMNode *stylesheet,
    /* [out][retval] */ BSTR *xmlString);


void __RPC_STUB IXMLDOMNode_transformNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_selectNodes_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ BSTR queryString,
    /* [out][retval] */ IXMLDOMNodeList **resultList);


void __RPC_STUB IXMLDOMNode_selectNodes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_selectSingleNode_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ BSTR queryString,
    /* [out][retval] */ IXMLDOMNode **resultNode);


void __RPC_STUB IXMLDOMNode_selectSingleNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_parsed_Proxy( 
    IXMLDOMNode * This,
    /* [out][retval] */ VARIANT_BOOL *isParsed);


void __RPC_STUB IXMLDOMNode_get_parsed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_namespaceURI_Proxy( 
    IXMLDOMNode * This,
    /* [out][retval] */ BSTR *namespaceURI);


void __RPC_STUB IXMLDOMNode_get_namespaceURI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_prefix_Proxy( 
    IXMLDOMNode * This,
    /* [out][retval] */ BSTR *prefixString);


void __RPC_STUB IXMLDOMNode_get_prefix_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_get_baseName_Proxy( 
    IXMLDOMNode * This,
    /* [out][retval] */ BSTR *nameString);


void __RPC_STUB IXMLDOMNode_get_baseName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNode_transformNodeToObject_Proxy( 
    IXMLDOMNode * This,
    /* [in] */ IXMLDOMNode *stylesheet,
    /* [in] */ VARIANT outputObject);


void __RPC_STUB IXMLDOMNode_transformNodeToObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMNode_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMDocumentFragment_INTERFACE_DEFINED__
#define __IXMLDOMDocumentFragment_INTERFACE_DEFINED__

/* interface IXMLDOMDocumentFragment */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMDocumentFragment;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3efaa413-272f-11d2-836f-0000f87a7782")
    IXMLDOMDocumentFragment : public IXMLDOMNode
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMDocumentFragmentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMDocumentFragment * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMDocumentFragment * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMDocumentFragment * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMDocumentFragment * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMDocumentFragment * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMDocumentFragment * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMDocumentFragment * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMDocumentFragment * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMDocumentFragment * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMDocumentFragment * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMDocumentFragment * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMDocumentFragment * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMDocumentFragment * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMDocumentFragment * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMDocumentFragment * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        END_INTERFACE
    } IXMLDOMDocumentFragmentVtbl;

    interface IXMLDOMDocumentFragment
    {
        CONST_VTBL struct IXMLDOMDocumentFragmentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMDocumentFragment_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMDocumentFragment_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMDocumentFragment_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMDocumentFragment_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMDocumentFragment_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMDocumentFragment_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMDocumentFragment_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMDocumentFragment_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMDocumentFragment_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMDocumentFragment_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMDocumentFragment_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMDocumentFragment_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMDocumentFragment_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMDocumentFragment_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMDocumentFragment_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMDocumentFragment_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMDocumentFragment_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMDocumentFragment_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMDocumentFragment_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMDocumentFragment_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMDocumentFragment_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMDocumentFragment_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMDocumentFragment_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMDocumentFragment_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMDocumentFragment_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMDocumentFragment_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMDocumentFragment_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMDocumentFragment_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMDocumentFragment_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMDocumentFragment_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMDocumentFragment_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMDocumentFragment_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMDocumentFragment_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMDocumentFragment_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMDocumentFragment_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMDocumentFragment_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMDocumentFragment_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMDocumentFragment_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMDocumentFragment_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMDocumentFragment_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMDocumentFragment_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMDocumentFragment_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMDocumentFragment_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IXMLDOMDocumentFragment_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMDocument_INTERFACE_DEFINED__
#define __IXMLDOMDocument_INTERFACE_DEFINED__

/* interface IXMLDOMDocument */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF81-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMDocument : public IXMLDOMNode
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_doctype( 
            /* [retval][out] */ IXMLDOMDocumentType **documentType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_implementation( 
            /* [retval][out] */ IXMLDOMImplementation **impl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_documentElement( 
            /* [retval][out] */ IXMLDOMElement **DOMElement) = 0;
        
        virtual /* [helpstring][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_documentElement( 
            /* [in] */ IXMLDOMElement *DOMElement) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createElement( 
            /* [in] */ BSTR tagName,
            /* [retval][out] */ IXMLDOMElement **element) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createDocumentFragment( 
            /* [retval][out] */ IXMLDOMDocumentFragment **docFrag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createTextNode( 
            /* [in] */ BSTR data,
            /* [retval][out] */ IXMLDOMText **text) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createComment( 
            /* [in] */ BSTR data,
            /* [retval][out] */ IXMLDOMComment **comment) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createCDATASection( 
            /* [in] */ BSTR data,
            /* [retval][out] */ IXMLDOMCDATASection **cdata) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createProcessingInstruction( 
            /* [in] */ BSTR target,
            /* [in] */ BSTR data,
            /* [retval][out] */ IXMLDOMProcessingInstruction **pi) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createAttribute( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IXMLDOMAttribute **attribute) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createEntityReference( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IXMLDOMEntityReference **entityRef) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getElementsByTagName( 
            /* [in] */ BSTR tagName,
            /* [retval][out] */ IXMLDOMNodeList **resultList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createNode( 
            /* [in] */ VARIANT Type,
            /* [in] */ BSTR name,
            /* [in] */ BSTR namespaceURI,
            /* [out][retval] */ IXMLDOMNode **node) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE nodeFromID( 
            /* [in] */ BSTR idString,
            /* [out][retval] */ IXMLDOMNode **node) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE load( 
            /* [in] */ VARIANT xmlSource,
            /* [retval][out] */ VARIANT_BOOL *isSuccessful) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_readyState( 
            /* [out][retval] */ long *value) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_parseError( 
            /* [out][retval] */ IXMLDOMParseError **errorObj) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_url( 
            /* [out][retval] */ BSTR *urlString) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_async( 
            /* [out][retval] */ VARIANT_BOOL *isAsync) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_async( 
            /* [in] */ VARIANT_BOOL isAsync) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE abort( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE loadXML( 
            /* [in] */ BSTR bstrXML,
            /* [retval][out] */ VARIANT_BOOL *isSuccessful) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE save( 
            /* [in] */ VARIANT destination) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_validateOnParse( 
            /* [out][retval] */ VARIANT_BOOL *isValidating) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_validateOnParse( 
            /* [in] */ VARIANT_BOOL isValidating) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_resolveExternals( 
            /* [out][retval] */ VARIANT_BOOL *isResolving) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_resolveExternals( 
            /* [in] */ VARIANT_BOOL isResolving) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_preserveWhiteSpace( 
            /* [out][retval] */ VARIANT_BOOL *isPreserving) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_preserveWhiteSpace( 
            /* [in] */ VARIANT_BOOL isPreserving) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_onreadystatechange( 
            /* [in] */ VARIANT readystatechangeSink) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ondataavailable( 
            /* [in] */ VARIANT ondataavailableSink) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ontransformnode( 
            /* [in] */ VARIANT ontransformnodeSink) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMDocument * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMDocument * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMDocument * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMDocument * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMDocument * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMDocument * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMDocument * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMDocument * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMDocument * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMDocument * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMDocument * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMDocument * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMDocument * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_doctype )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMDocumentType **documentType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_implementation )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMImplementation **impl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_documentElement )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMElement **DOMElement);
        
        /* [helpstring][id][propputref] */ HRESULT ( STDMETHODCALLTYPE *putref_documentElement )( 
            IXMLDOMDocument * This,
            /* [in] */ IXMLDOMElement *DOMElement);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createElement )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR tagName,
            /* [retval][out] */ IXMLDOMElement **element);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createDocumentFragment )( 
            IXMLDOMDocument * This,
            /* [retval][out] */ IXMLDOMDocumentFragment **docFrag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createTextNode )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR data,
            /* [retval][out] */ IXMLDOMText **text);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createComment )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR data,
            /* [retval][out] */ IXMLDOMComment **comment);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createCDATASection )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR data,
            /* [retval][out] */ IXMLDOMCDATASection **cdata);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createProcessingInstruction )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR target,
            /* [in] */ BSTR data,
            /* [retval][out] */ IXMLDOMProcessingInstruction **pi);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createAttribute )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IXMLDOMAttribute **attribute);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createEntityReference )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IXMLDOMEntityReference **entityRef);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getElementsByTagName )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR tagName,
            /* [retval][out] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createNode )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT Type,
            /* [in] */ BSTR name,
            /* [in] */ BSTR namespaceURI,
            /* [out][retval] */ IXMLDOMNode **node);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *nodeFromID )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR idString,
            /* [out][retval] */ IXMLDOMNode **node);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *load )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT xmlSource,
            /* [retval][out] */ VARIANT_BOOL *isSuccessful);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ long *value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parseError )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ IXMLDOMParseError **errorObj);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_url )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ BSTR *urlString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_async )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ VARIANT_BOOL *isAsync);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_async )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT_BOOL isAsync);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *abort )( 
            IXMLDOMDocument * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *loadXML )( 
            IXMLDOMDocument * This,
            /* [in] */ BSTR bstrXML,
            /* [retval][out] */ VARIANT_BOOL *isSuccessful);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *save )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT destination);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_validateOnParse )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ VARIANT_BOOL *isValidating);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_validateOnParse )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT_BOOL isValidating);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_resolveExternals )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ VARIANT_BOOL *isResolving);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_resolveExternals )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT_BOOL isResolving);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_preserveWhiteSpace )( 
            IXMLDOMDocument * This,
            /* [out][retval] */ VARIANT_BOOL *isPreserving);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_preserveWhiteSpace )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT_BOOL isPreserving);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_onreadystatechange )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT readystatechangeSink);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ondataavailable )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT ondataavailableSink);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ontransformnode )( 
            IXMLDOMDocument * This,
            /* [in] */ VARIANT ontransformnodeSink);
        
        END_INTERFACE
    } IXMLDOMDocumentVtbl;

    interface IXMLDOMDocument
    {
        CONST_VTBL struct IXMLDOMDocumentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMDocument_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMDocument_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMDocument_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMDocument_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMDocument_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMDocument_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMDocument_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMDocument_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMDocument_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMDocument_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMDocument_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMDocument_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMDocument_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMDocument_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMDocument_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMDocument_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMDocument_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMDocument_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMDocument_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMDocument_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMDocument_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMDocument_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMDocument_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMDocument_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMDocument_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMDocument_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMDocument_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMDocument_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMDocument_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMDocument_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMDocument_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMDocument_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMDocument_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMDocument_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMDocument_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMDocument_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMDocument_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMDocument_get_doctype(This,documentType)	\
    (This)->lpVtbl -> get_doctype(This,documentType)

#define IXMLDOMDocument_get_implementation(This,impl)	\
    (This)->lpVtbl -> get_implementation(This,impl)

#define IXMLDOMDocument_get_documentElement(This,DOMElement)	\
    (This)->lpVtbl -> get_documentElement(This,DOMElement)

#define IXMLDOMDocument_putref_documentElement(This,DOMElement)	\
    (This)->lpVtbl -> putref_documentElement(This,DOMElement)

#define IXMLDOMDocument_createElement(This,tagName,element)	\
    (This)->lpVtbl -> createElement(This,tagName,element)

#define IXMLDOMDocument_createDocumentFragment(This,docFrag)	\
    (This)->lpVtbl -> createDocumentFragment(This,docFrag)

#define IXMLDOMDocument_createTextNode(This,data,text)	\
    (This)->lpVtbl -> createTextNode(This,data,text)

#define IXMLDOMDocument_createComment(This,data,comment)	\
    (This)->lpVtbl -> createComment(This,data,comment)

#define IXMLDOMDocument_createCDATASection(This,data,cdata)	\
    (This)->lpVtbl -> createCDATASection(This,data,cdata)

#define IXMLDOMDocument_createProcessingInstruction(This,target,data,pi)	\
    (This)->lpVtbl -> createProcessingInstruction(This,target,data,pi)

#define IXMLDOMDocument_createAttribute(This,name,attribute)	\
    (This)->lpVtbl -> createAttribute(This,name,attribute)

#define IXMLDOMDocument_createEntityReference(This,name,entityRef)	\
    (This)->lpVtbl -> createEntityReference(This,name,entityRef)

#define IXMLDOMDocument_getElementsByTagName(This,tagName,resultList)	\
    (This)->lpVtbl -> getElementsByTagName(This,tagName,resultList)

#define IXMLDOMDocument_createNode(This,Type,name,namespaceURI,node)	\
    (This)->lpVtbl -> createNode(This,Type,name,namespaceURI,node)

#define IXMLDOMDocument_nodeFromID(This,idString,node)	\
    (This)->lpVtbl -> nodeFromID(This,idString,node)

#define IXMLDOMDocument_load(This,xmlSource,isSuccessful)	\
    (This)->lpVtbl -> load(This,xmlSource,isSuccessful)

#define IXMLDOMDocument_get_readyState(This,value)	\
    (This)->lpVtbl -> get_readyState(This,value)

#define IXMLDOMDocument_get_parseError(This,errorObj)	\
    (This)->lpVtbl -> get_parseError(This,errorObj)

#define IXMLDOMDocument_get_url(This,urlString)	\
    (This)->lpVtbl -> get_url(This,urlString)

#define IXMLDOMDocument_get_async(This,isAsync)	\
    (This)->lpVtbl -> get_async(This,isAsync)

#define IXMLDOMDocument_put_async(This,isAsync)	\
    (This)->lpVtbl -> put_async(This,isAsync)

#define IXMLDOMDocument_abort(This)	\
    (This)->lpVtbl -> abort(This)

#define IXMLDOMDocument_loadXML(This,bstrXML,isSuccessful)	\
    (This)->lpVtbl -> loadXML(This,bstrXML,isSuccessful)

#define IXMLDOMDocument_save(This,destination)	\
    (This)->lpVtbl -> save(This,destination)

#define IXMLDOMDocument_get_validateOnParse(This,isValidating)	\
    (This)->lpVtbl -> get_validateOnParse(This,isValidating)

#define IXMLDOMDocument_put_validateOnParse(This,isValidating)	\
    (This)->lpVtbl -> put_validateOnParse(This,isValidating)

#define IXMLDOMDocument_get_resolveExternals(This,isResolving)	\
    (This)->lpVtbl -> get_resolveExternals(This,isResolving)

#define IXMLDOMDocument_put_resolveExternals(This,isResolving)	\
    (This)->lpVtbl -> put_resolveExternals(This,isResolving)

#define IXMLDOMDocument_get_preserveWhiteSpace(This,isPreserving)	\
    (This)->lpVtbl -> get_preserveWhiteSpace(This,isPreserving)

#define IXMLDOMDocument_put_preserveWhiteSpace(This,isPreserving)	\
    (This)->lpVtbl -> put_preserveWhiteSpace(This,isPreserving)

#define IXMLDOMDocument_put_onreadystatechange(This,readystatechangeSink)	\
    (This)->lpVtbl -> put_onreadystatechange(This,readystatechangeSink)

#define IXMLDOMDocument_put_ondataavailable(This,ondataavailableSink)	\
    (This)->lpVtbl -> put_ondataavailable(This,ondataavailableSink)

#define IXMLDOMDocument_put_ontransformnode(This,ontransformnodeSink)	\
    (This)->lpVtbl -> put_ontransformnode(This,ontransformnodeSink)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_get_doctype_Proxy( 
    IXMLDOMDocument * This,
    /* [retval][out] */ IXMLDOMDocumentType **documentType);


void __RPC_STUB IXMLDOMDocument_get_doctype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_get_implementation_Proxy( 
    IXMLDOMDocument * This,
    /* [retval][out] */ IXMLDOMImplementation **impl);


void __RPC_STUB IXMLDOMDocument_get_implementation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_get_documentElement_Proxy( 
    IXMLDOMDocument * This,
    /* [retval][out] */ IXMLDOMElement **DOMElement);


void __RPC_STUB IXMLDOMDocument_get_documentElement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propputref] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_putref_documentElement_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ IXMLDOMElement *DOMElement);


void __RPC_STUB IXMLDOMDocument_putref_documentElement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_createElement_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ BSTR tagName,
    /* [retval][out] */ IXMLDOMElement **element);


void __RPC_STUB IXMLDOMDocument_createElement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_createDocumentFragment_Proxy( 
    IXMLDOMDocument * This,
    /* [retval][out] */ IXMLDOMDocumentFragment **docFrag);


void __RPC_STUB IXMLDOMDocument_createDocumentFragment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_createTextNode_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ BSTR data,
    /* [retval][out] */ IXMLDOMText **text);


void __RPC_STUB IXMLDOMDocument_createTextNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_createComment_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ BSTR data,
    /* [retval][out] */ IXMLDOMComment **comment);


void __RPC_STUB IXMLDOMDocument_createComment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_createCDATASection_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ BSTR data,
    /* [retval][out] */ IXMLDOMCDATASection **cdata);


void __RPC_STUB IXMLDOMDocument_createCDATASection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_createProcessingInstruction_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ BSTR target,
    /* [in] */ BSTR data,
    /* [retval][out] */ IXMLDOMProcessingInstruction **pi);


void __RPC_STUB IXMLDOMDocument_createProcessingInstruction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_createAttribute_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IXMLDOMAttribute **attribute);


void __RPC_STUB IXMLDOMDocument_createAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_createEntityReference_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IXMLDOMEntityReference **entityRef);


void __RPC_STUB IXMLDOMDocument_createEntityReference_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_getElementsByTagName_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ BSTR tagName,
    /* [retval][out] */ IXMLDOMNodeList **resultList);


void __RPC_STUB IXMLDOMDocument_getElementsByTagName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_createNode_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ VARIANT Type,
    /* [in] */ BSTR name,
    /* [in] */ BSTR namespaceURI,
    /* [out][retval] */ IXMLDOMNode **node);


void __RPC_STUB IXMLDOMDocument_createNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_nodeFromID_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ BSTR idString,
    /* [out][retval] */ IXMLDOMNode **node);


void __RPC_STUB IXMLDOMDocument_nodeFromID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_load_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ VARIANT xmlSource,
    /* [retval][out] */ VARIANT_BOOL *isSuccessful);


void __RPC_STUB IXMLDOMDocument_load_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_get_readyState_Proxy( 
    IXMLDOMDocument * This,
    /* [out][retval] */ long *value);


void __RPC_STUB IXMLDOMDocument_get_readyState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_get_parseError_Proxy( 
    IXMLDOMDocument * This,
    /* [out][retval] */ IXMLDOMParseError **errorObj);


void __RPC_STUB IXMLDOMDocument_get_parseError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_get_url_Proxy( 
    IXMLDOMDocument * This,
    /* [out][retval] */ BSTR *urlString);


void __RPC_STUB IXMLDOMDocument_get_url_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_get_async_Proxy( 
    IXMLDOMDocument * This,
    /* [out][retval] */ VARIANT_BOOL *isAsync);


void __RPC_STUB IXMLDOMDocument_get_async_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_put_async_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ VARIANT_BOOL isAsync);


void __RPC_STUB IXMLDOMDocument_put_async_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_abort_Proxy( 
    IXMLDOMDocument * This);


void __RPC_STUB IXMLDOMDocument_abort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_loadXML_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ BSTR bstrXML,
    /* [retval][out] */ VARIANT_BOOL *isSuccessful);


void __RPC_STUB IXMLDOMDocument_loadXML_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_save_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ VARIANT destination);


void __RPC_STUB IXMLDOMDocument_save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_get_validateOnParse_Proxy( 
    IXMLDOMDocument * This,
    /* [out][retval] */ VARIANT_BOOL *isValidating);


void __RPC_STUB IXMLDOMDocument_get_validateOnParse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_put_validateOnParse_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ VARIANT_BOOL isValidating);


void __RPC_STUB IXMLDOMDocument_put_validateOnParse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_get_resolveExternals_Proxy( 
    IXMLDOMDocument * This,
    /* [out][retval] */ VARIANT_BOOL *isResolving);


void __RPC_STUB IXMLDOMDocument_get_resolveExternals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_put_resolveExternals_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ VARIANT_BOOL isResolving);


void __RPC_STUB IXMLDOMDocument_put_resolveExternals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_get_preserveWhiteSpace_Proxy( 
    IXMLDOMDocument * This,
    /* [out][retval] */ VARIANT_BOOL *isPreserving);


void __RPC_STUB IXMLDOMDocument_get_preserveWhiteSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_put_preserveWhiteSpace_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ VARIANT_BOOL isPreserving);


void __RPC_STUB IXMLDOMDocument_put_preserveWhiteSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_put_onreadystatechange_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ VARIANT readystatechangeSink);


void __RPC_STUB IXMLDOMDocument_put_onreadystatechange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_put_ondataavailable_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ VARIANT ondataavailableSink);


void __RPC_STUB IXMLDOMDocument_put_ondataavailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocument_put_ontransformnode_Proxy( 
    IXMLDOMDocument * This,
    /* [in] */ VARIANT ontransformnodeSink);


void __RPC_STUB IXMLDOMDocument_put_ontransformnode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMDocument_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMNodeList_INTERFACE_DEFINED__
#define __IXMLDOMNodeList_INTERFACE_DEFINED__

/* interface IXMLDOMNodeList */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMNodeList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF82-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMNodeList : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_item( 
            /* [in] */ long index,
            /* [retval][out] */ IXMLDOMNode **listItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_length( 
            /* [retval][out] */ long *listLength) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE nextNode( 
            /* [retval][out] */ IXMLDOMNode **nextItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE reset( void) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__newEnum( 
            /* [out][retval] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMNodeListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMNodeList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMNodeList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMNodeList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMNodeList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMNodeList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMNodeList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMNodeList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_item )( 
            IXMLDOMNodeList * This,
            /* [in] */ long index,
            /* [retval][out] */ IXMLDOMNode **listItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMNodeList * This,
            /* [retval][out] */ long *listLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *nextNode )( 
            IXMLDOMNodeList * This,
            /* [retval][out] */ IXMLDOMNode **nextItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *reset )( 
            IXMLDOMNodeList * This);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            IXMLDOMNodeList * This,
            /* [out][retval] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } IXMLDOMNodeListVtbl;

    interface IXMLDOMNodeList
    {
        CONST_VTBL struct IXMLDOMNodeListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMNodeList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMNodeList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMNodeList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMNodeList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMNodeList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMNodeList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMNodeList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMNodeList_get_item(This,index,listItem)	\
    (This)->lpVtbl -> get_item(This,index,listItem)

#define IXMLDOMNodeList_get_length(This,listLength)	\
    (This)->lpVtbl -> get_length(This,listLength)

#define IXMLDOMNodeList_nextNode(This,nextItem)	\
    (This)->lpVtbl -> nextNode(This,nextItem)

#define IXMLDOMNodeList_reset(This)	\
    (This)->lpVtbl -> reset(This)

#define IXMLDOMNodeList_get__newEnum(This,ppUnk)	\
    (This)->lpVtbl -> get__newEnum(This,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNodeList_get_item_Proxy( 
    IXMLDOMNodeList * This,
    /* [in] */ long index,
    /* [retval][out] */ IXMLDOMNode **listItem);


void __RPC_STUB IXMLDOMNodeList_get_item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNodeList_get_length_Proxy( 
    IXMLDOMNodeList * This,
    /* [retval][out] */ long *listLength);


void __RPC_STUB IXMLDOMNodeList_get_length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNodeList_nextNode_Proxy( 
    IXMLDOMNodeList * This,
    /* [retval][out] */ IXMLDOMNode **nextItem);


void __RPC_STUB IXMLDOMNodeList_nextNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNodeList_reset_Proxy( 
    IXMLDOMNodeList * This);


void __RPC_STUB IXMLDOMNodeList_reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNodeList_get__newEnum_Proxy( 
    IXMLDOMNodeList * This,
    /* [out][retval] */ IUnknown **ppUnk);


void __RPC_STUB IXMLDOMNodeList_get__newEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMNodeList_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMNamedNodeMap_INTERFACE_DEFINED__
#define __IXMLDOMNamedNodeMap_INTERFACE_DEFINED__

/* interface IXMLDOMNamedNodeMap */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMNamedNodeMap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF83-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMNamedNodeMap : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getNamedItem( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IXMLDOMNode **namedItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setNamedItem( 
            /* [in] */ IXMLDOMNode *newItem,
            /* [retval][out] */ IXMLDOMNode **nameItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE removeNamedItem( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IXMLDOMNode **namedItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_item( 
            /* [in] */ long index,
            /* [retval][out] */ IXMLDOMNode **listItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_length( 
            /* [retval][out] */ long *listLength) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getQualifiedItem( 
            /* [in] */ BSTR baseName,
            /* [in] */ BSTR namespaceURI,
            /* [retval][out] */ IXMLDOMNode **qualifiedItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE removeQualifiedItem( 
            /* [in] */ BSTR baseName,
            /* [in] */ BSTR namespaceURI,
            /* [retval][out] */ IXMLDOMNode **qualifiedItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE nextNode( 
            /* [retval][out] */ IXMLDOMNode **nextItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE reset( void) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__newEnum( 
            /* [out][retval] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMNamedNodeMapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMNamedNodeMap * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMNamedNodeMap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMNamedNodeMap * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMNamedNodeMap * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMNamedNodeMap * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMNamedNodeMap * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMNamedNodeMap * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getNamedItem )( 
            IXMLDOMNamedNodeMap * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IXMLDOMNode **namedItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setNamedItem )( 
            IXMLDOMNamedNodeMap * This,
            /* [in] */ IXMLDOMNode *newItem,
            /* [retval][out] */ IXMLDOMNode **nameItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeNamedItem )( 
            IXMLDOMNamedNodeMap * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IXMLDOMNode **namedItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_item )( 
            IXMLDOMNamedNodeMap * This,
            /* [in] */ long index,
            /* [retval][out] */ IXMLDOMNode **listItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMNamedNodeMap * This,
            /* [retval][out] */ long *listLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getQualifiedItem )( 
            IXMLDOMNamedNodeMap * This,
            /* [in] */ BSTR baseName,
            /* [in] */ BSTR namespaceURI,
            /* [retval][out] */ IXMLDOMNode **qualifiedItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeQualifiedItem )( 
            IXMLDOMNamedNodeMap * This,
            /* [in] */ BSTR baseName,
            /* [in] */ BSTR namespaceURI,
            /* [retval][out] */ IXMLDOMNode **qualifiedItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *nextNode )( 
            IXMLDOMNamedNodeMap * This,
            /* [retval][out] */ IXMLDOMNode **nextItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *reset )( 
            IXMLDOMNamedNodeMap * This);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            IXMLDOMNamedNodeMap * This,
            /* [out][retval] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } IXMLDOMNamedNodeMapVtbl;

    interface IXMLDOMNamedNodeMap
    {
        CONST_VTBL struct IXMLDOMNamedNodeMapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMNamedNodeMap_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMNamedNodeMap_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMNamedNodeMap_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMNamedNodeMap_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMNamedNodeMap_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMNamedNodeMap_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMNamedNodeMap_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMNamedNodeMap_getNamedItem(This,name,namedItem)	\
    (This)->lpVtbl -> getNamedItem(This,name,namedItem)

#define IXMLDOMNamedNodeMap_setNamedItem(This,newItem,nameItem)	\
    (This)->lpVtbl -> setNamedItem(This,newItem,nameItem)

#define IXMLDOMNamedNodeMap_removeNamedItem(This,name,namedItem)	\
    (This)->lpVtbl -> removeNamedItem(This,name,namedItem)

#define IXMLDOMNamedNodeMap_get_item(This,index,listItem)	\
    (This)->lpVtbl -> get_item(This,index,listItem)

#define IXMLDOMNamedNodeMap_get_length(This,listLength)	\
    (This)->lpVtbl -> get_length(This,listLength)

#define IXMLDOMNamedNodeMap_getQualifiedItem(This,baseName,namespaceURI,qualifiedItem)	\
    (This)->lpVtbl -> getQualifiedItem(This,baseName,namespaceURI,qualifiedItem)

#define IXMLDOMNamedNodeMap_removeQualifiedItem(This,baseName,namespaceURI,qualifiedItem)	\
    (This)->lpVtbl -> removeQualifiedItem(This,baseName,namespaceURI,qualifiedItem)

#define IXMLDOMNamedNodeMap_nextNode(This,nextItem)	\
    (This)->lpVtbl -> nextNode(This,nextItem)

#define IXMLDOMNamedNodeMap_reset(This)	\
    (This)->lpVtbl -> reset(This)

#define IXMLDOMNamedNodeMap_get__newEnum(This,ppUnk)	\
    (This)->lpVtbl -> get__newEnum(This,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNamedNodeMap_getNamedItem_Proxy( 
    IXMLDOMNamedNodeMap * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IXMLDOMNode **namedItem);


void __RPC_STUB IXMLDOMNamedNodeMap_getNamedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNamedNodeMap_setNamedItem_Proxy( 
    IXMLDOMNamedNodeMap * This,
    /* [in] */ IXMLDOMNode *newItem,
    /* [retval][out] */ IXMLDOMNode **nameItem);


void __RPC_STUB IXMLDOMNamedNodeMap_setNamedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNamedNodeMap_removeNamedItem_Proxy( 
    IXMLDOMNamedNodeMap * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IXMLDOMNode **namedItem);


void __RPC_STUB IXMLDOMNamedNodeMap_removeNamedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNamedNodeMap_get_item_Proxy( 
    IXMLDOMNamedNodeMap * This,
    /* [in] */ long index,
    /* [retval][out] */ IXMLDOMNode **listItem);


void __RPC_STUB IXMLDOMNamedNodeMap_get_item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNamedNodeMap_get_length_Proxy( 
    IXMLDOMNamedNodeMap * This,
    /* [retval][out] */ long *listLength);


void __RPC_STUB IXMLDOMNamedNodeMap_get_length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNamedNodeMap_getQualifiedItem_Proxy( 
    IXMLDOMNamedNodeMap * This,
    /* [in] */ BSTR baseName,
    /* [in] */ BSTR namespaceURI,
    /* [retval][out] */ IXMLDOMNode **qualifiedItem);


void __RPC_STUB IXMLDOMNamedNodeMap_getQualifiedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNamedNodeMap_removeQualifiedItem_Proxy( 
    IXMLDOMNamedNodeMap * This,
    /* [in] */ BSTR baseName,
    /* [in] */ BSTR namespaceURI,
    /* [retval][out] */ IXMLDOMNode **qualifiedItem);


void __RPC_STUB IXMLDOMNamedNodeMap_removeQualifiedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNamedNodeMap_nextNode_Proxy( 
    IXMLDOMNamedNodeMap * This,
    /* [retval][out] */ IXMLDOMNode **nextItem);


void __RPC_STUB IXMLDOMNamedNodeMap_nextNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMNamedNodeMap_reset_Proxy( 
    IXMLDOMNamedNodeMap * This);


void __RPC_STUB IXMLDOMNamedNodeMap_reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNamedNodeMap_get__newEnum_Proxy( 
    IXMLDOMNamedNodeMap * This,
    /* [out][retval] */ IUnknown **ppUnk);


void __RPC_STUB IXMLDOMNamedNodeMap_get__newEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMNamedNodeMap_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMCharacterData_INTERFACE_DEFINED__
#define __IXMLDOMCharacterData_INTERFACE_DEFINED__

/* interface IXMLDOMCharacterData */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMCharacterData;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF84-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMCharacterData : public IXMLDOMNode
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_data( 
            /* [retval][out] */ BSTR *data) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_data( 
            /* [in] */ BSTR data) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_length( 
            /* [retval][out] */ long *dataLength) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE substringData( 
            /* [in] */ long offset,
            /* [in] */ long count,
            /* [retval][out] */ BSTR *data) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE appendData( 
            /* [in] */ BSTR data) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE insertData( 
            /* [in] */ long offset,
            /* [in] */ BSTR data) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE deleteData( 
            /* [in] */ long offset,
            /* [in] */ long count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE replaceData( 
            /* [in] */ long offset,
            /* [in] */ long count,
            /* [in] */ BSTR data) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMCharacterDataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMCharacterData * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMCharacterData * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMCharacterData * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMCharacterData * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMCharacterData * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMCharacterData * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMCharacterData * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMCharacterData * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMCharacterData * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMCharacterData * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMCharacterData * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMCharacterData * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMCharacterData * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMCharacterData * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMCharacterData * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMCharacterData * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMCharacterData * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMCharacterData * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMCharacterData * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMCharacterData * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMCharacterData * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMCharacterData * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMCharacterData * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMCharacterData * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMCharacterData * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMCharacterData * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMCharacterData * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMCharacterData * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMCharacterData * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMCharacterData * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_data )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ BSTR *data);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_data )( 
            IXMLDOMCharacterData * This,
            /* [in] */ BSTR data);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMCharacterData * This,
            /* [retval][out] */ long *dataLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *substringData )( 
            IXMLDOMCharacterData * This,
            /* [in] */ long offset,
            /* [in] */ long count,
            /* [retval][out] */ BSTR *data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendData )( 
            IXMLDOMCharacterData * This,
            /* [in] */ BSTR data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertData )( 
            IXMLDOMCharacterData * This,
            /* [in] */ long offset,
            /* [in] */ BSTR data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *deleteData )( 
            IXMLDOMCharacterData * This,
            /* [in] */ long offset,
            /* [in] */ long count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceData )( 
            IXMLDOMCharacterData * This,
            /* [in] */ long offset,
            /* [in] */ long count,
            /* [in] */ BSTR data);
        
        END_INTERFACE
    } IXMLDOMCharacterDataVtbl;

    interface IXMLDOMCharacterData
    {
        CONST_VTBL struct IXMLDOMCharacterDataVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMCharacterData_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMCharacterData_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMCharacterData_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMCharacterData_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMCharacterData_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMCharacterData_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMCharacterData_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMCharacterData_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMCharacterData_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMCharacterData_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMCharacterData_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMCharacterData_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMCharacterData_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMCharacterData_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMCharacterData_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMCharacterData_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMCharacterData_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMCharacterData_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMCharacterData_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMCharacterData_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMCharacterData_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMCharacterData_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMCharacterData_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMCharacterData_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMCharacterData_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMCharacterData_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMCharacterData_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMCharacterData_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMCharacterData_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMCharacterData_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMCharacterData_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMCharacterData_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMCharacterData_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMCharacterData_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMCharacterData_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMCharacterData_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMCharacterData_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMCharacterData_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMCharacterData_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMCharacterData_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMCharacterData_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMCharacterData_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMCharacterData_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMCharacterData_get_data(This,data)	\
    (This)->lpVtbl -> get_data(This,data)

#define IXMLDOMCharacterData_put_data(This,data)	\
    (This)->lpVtbl -> put_data(This,data)

#define IXMLDOMCharacterData_get_length(This,dataLength)	\
    (This)->lpVtbl -> get_length(This,dataLength)

#define IXMLDOMCharacterData_substringData(This,offset,count,data)	\
    (This)->lpVtbl -> substringData(This,offset,count,data)

#define IXMLDOMCharacterData_appendData(This,data)	\
    (This)->lpVtbl -> appendData(This,data)

#define IXMLDOMCharacterData_insertData(This,offset,data)	\
    (This)->lpVtbl -> insertData(This,offset,data)

#define IXMLDOMCharacterData_deleteData(This,offset,count)	\
    (This)->lpVtbl -> deleteData(This,offset,count)

#define IXMLDOMCharacterData_replaceData(This,offset,count,data)	\
    (This)->lpVtbl -> replaceData(This,offset,count,data)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMCharacterData_get_data_Proxy( 
    IXMLDOMCharacterData * This,
    /* [retval][out] */ BSTR *data);


void __RPC_STUB IXMLDOMCharacterData_get_data_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMCharacterData_put_data_Proxy( 
    IXMLDOMCharacterData * This,
    /* [in] */ BSTR data);


void __RPC_STUB IXMLDOMCharacterData_put_data_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMCharacterData_get_length_Proxy( 
    IXMLDOMCharacterData * This,
    /* [retval][out] */ long *dataLength);


void __RPC_STUB IXMLDOMCharacterData_get_length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMCharacterData_substringData_Proxy( 
    IXMLDOMCharacterData * This,
    /* [in] */ long offset,
    /* [in] */ long count,
    /* [retval][out] */ BSTR *data);


void __RPC_STUB IXMLDOMCharacterData_substringData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMCharacterData_appendData_Proxy( 
    IXMLDOMCharacterData * This,
    /* [in] */ BSTR data);


void __RPC_STUB IXMLDOMCharacterData_appendData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMCharacterData_insertData_Proxy( 
    IXMLDOMCharacterData * This,
    /* [in] */ long offset,
    /* [in] */ BSTR data);


void __RPC_STUB IXMLDOMCharacterData_insertData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMCharacterData_deleteData_Proxy( 
    IXMLDOMCharacterData * This,
    /* [in] */ long offset,
    /* [in] */ long count);


void __RPC_STUB IXMLDOMCharacterData_deleteData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMCharacterData_replaceData_Proxy( 
    IXMLDOMCharacterData * This,
    /* [in] */ long offset,
    /* [in] */ long count,
    /* [in] */ BSTR data);


void __RPC_STUB IXMLDOMCharacterData_replaceData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMCharacterData_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMAttribute_INTERFACE_DEFINED__
#define __IXMLDOMAttribute_INTERFACE_DEFINED__

/* interface IXMLDOMAttribute */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMAttribute;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF85-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMAttribute : public IXMLDOMNode
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_name( 
            /* [retval][out] */ BSTR *attributeName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_value( 
            /* [retval][out] */ VARIANT *attributeValue) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_value( 
            /* [in] */ VARIANT attributeValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMAttributeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMAttribute * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMAttribute * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMAttribute * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMAttribute * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMAttribute * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMAttribute * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMAttribute * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMAttribute * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMAttribute * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMAttribute * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMAttribute * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMAttribute * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMAttribute * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMAttribute * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMAttribute * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMAttribute * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMAttribute * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMAttribute * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMAttribute * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMAttribute * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMAttribute * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMAttribute * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMAttribute * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMAttribute * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMAttribute * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMAttribute * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMAttribute * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMAttribute * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMAttribute * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMAttribute * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_name )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ BSTR *attributeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_value )( 
            IXMLDOMAttribute * This,
            /* [retval][out] */ VARIANT *attributeValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_value )( 
            IXMLDOMAttribute * This,
            /* [in] */ VARIANT attributeValue);
        
        END_INTERFACE
    } IXMLDOMAttributeVtbl;

    interface IXMLDOMAttribute
    {
        CONST_VTBL struct IXMLDOMAttributeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMAttribute_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMAttribute_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMAttribute_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMAttribute_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMAttribute_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMAttribute_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMAttribute_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMAttribute_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMAttribute_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMAttribute_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMAttribute_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMAttribute_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMAttribute_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMAttribute_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMAttribute_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMAttribute_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMAttribute_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMAttribute_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMAttribute_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMAttribute_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMAttribute_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMAttribute_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMAttribute_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMAttribute_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMAttribute_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMAttribute_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMAttribute_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMAttribute_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMAttribute_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMAttribute_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMAttribute_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMAttribute_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMAttribute_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMAttribute_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMAttribute_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMAttribute_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMAttribute_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMAttribute_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMAttribute_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMAttribute_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMAttribute_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMAttribute_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMAttribute_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMAttribute_get_name(This,attributeName)	\
    (This)->lpVtbl -> get_name(This,attributeName)

#define IXMLDOMAttribute_get_value(This,attributeValue)	\
    (This)->lpVtbl -> get_value(This,attributeValue)

#define IXMLDOMAttribute_put_value(This,attributeValue)	\
    (This)->lpVtbl -> put_value(This,attributeValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMAttribute_get_name_Proxy( 
    IXMLDOMAttribute * This,
    /* [retval][out] */ BSTR *attributeName);


void __RPC_STUB IXMLDOMAttribute_get_name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMAttribute_get_value_Proxy( 
    IXMLDOMAttribute * This,
    /* [retval][out] */ VARIANT *attributeValue);


void __RPC_STUB IXMLDOMAttribute_get_value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMAttribute_put_value_Proxy( 
    IXMLDOMAttribute * This,
    /* [in] */ VARIANT attributeValue);


void __RPC_STUB IXMLDOMAttribute_put_value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMAttribute_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMElement_INTERFACE_DEFINED__
#define __IXMLDOMElement_INTERFACE_DEFINED__

/* interface IXMLDOMElement */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMElement;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF86-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMElement : public IXMLDOMNode
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_tagName( 
            /* [retval][out] */ BSTR *tagName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getAttribute( 
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setAttribute( 
            /* [in] */ BSTR name,
            /* [in] */ VARIANT value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE removeAttribute( 
            /* [in] */ BSTR name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getAttributeNode( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IXMLDOMAttribute **attributeNode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setAttributeNode( 
            /* [in] */ IXMLDOMAttribute *DOMAttribute,
            /* [retval][out] */ IXMLDOMAttribute **attributeNode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE removeAttributeNode( 
            /* [in] */ IXMLDOMAttribute *DOMAttribute,
            /* [retval][out] */ IXMLDOMAttribute **attributeNode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getElementsByTagName( 
            /* [in] */ BSTR tagName,
            /* [retval][out] */ IXMLDOMNodeList **resultList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE normalize( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMElementVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMElement * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMElement * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMElement * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMElement * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMElement * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMElement * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMElement * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMElement * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMElement * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMElement * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMElement * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMElement * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMElement * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMElement * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMElement * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMElement * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMElement * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMElement * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMElement * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMElement * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMElement * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMElement * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMElement * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMElement * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMElement * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMElement * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMElement * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMElement * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMElement * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMElement * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMElement * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMElement * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMElement * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMElement * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMElement * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMElement * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMElement * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMElement * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMElement * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMElement * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMElement * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMElement * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMElement * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_tagName )( 
            IXMLDOMElement * This,
            /* [retval][out] */ BSTR *tagName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getAttribute )( 
            IXMLDOMElement * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setAttribute )( 
            IXMLDOMElement * This,
            /* [in] */ BSTR name,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeAttribute )( 
            IXMLDOMElement * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getAttributeNode )( 
            IXMLDOMElement * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IXMLDOMAttribute **attributeNode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setAttributeNode )( 
            IXMLDOMElement * This,
            /* [in] */ IXMLDOMAttribute *DOMAttribute,
            /* [retval][out] */ IXMLDOMAttribute **attributeNode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeAttributeNode )( 
            IXMLDOMElement * This,
            /* [in] */ IXMLDOMAttribute *DOMAttribute,
            /* [retval][out] */ IXMLDOMAttribute **attributeNode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getElementsByTagName )( 
            IXMLDOMElement * This,
            /* [in] */ BSTR tagName,
            /* [retval][out] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *normalize )( 
            IXMLDOMElement * This);
        
        END_INTERFACE
    } IXMLDOMElementVtbl;

    interface IXMLDOMElement
    {
        CONST_VTBL struct IXMLDOMElementVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMElement_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMElement_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMElement_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMElement_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMElement_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMElement_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMElement_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMElement_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMElement_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMElement_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMElement_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMElement_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMElement_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMElement_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMElement_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMElement_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMElement_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMElement_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMElement_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMElement_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMElement_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMElement_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMElement_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMElement_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMElement_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMElement_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMElement_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMElement_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMElement_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMElement_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMElement_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMElement_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMElement_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMElement_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMElement_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMElement_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMElement_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMElement_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMElement_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMElement_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMElement_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMElement_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMElement_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMElement_get_tagName(This,tagName)	\
    (This)->lpVtbl -> get_tagName(This,tagName)

#define IXMLDOMElement_getAttribute(This,name,value)	\
    (This)->lpVtbl -> getAttribute(This,name,value)

#define IXMLDOMElement_setAttribute(This,name,value)	\
    (This)->lpVtbl -> setAttribute(This,name,value)

#define IXMLDOMElement_removeAttribute(This,name)	\
    (This)->lpVtbl -> removeAttribute(This,name)

#define IXMLDOMElement_getAttributeNode(This,name,attributeNode)	\
    (This)->lpVtbl -> getAttributeNode(This,name,attributeNode)

#define IXMLDOMElement_setAttributeNode(This,DOMAttribute,attributeNode)	\
    (This)->lpVtbl -> setAttributeNode(This,DOMAttribute,attributeNode)

#define IXMLDOMElement_removeAttributeNode(This,DOMAttribute,attributeNode)	\
    (This)->lpVtbl -> removeAttributeNode(This,DOMAttribute,attributeNode)

#define IXMLDOMElement_getElementsByTagName(This,tagName,resultList)	\
    (This)->lpVtbl -> getElementsByTagName(This,tagName,resultList)

#define IXMLDOMElement_normalize(This)	\
    (This)->lpVtbl -> normalize(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMElement_get_tagName_Proxy( 
    IXMLDOMElement * This,
    /* [retval][out] */ BSTR *tagName);


void __RPC_STUB IXMLDOMElement_get_tagName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMElement_getAttribute_Proxy( 
    IXMLDOMElement * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ VARIANT *value);


void __RPC_STUB IXMLDOMElement_getAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMElement_setAttribute_Proxy( 
    IXMLDOMElement * This,
    /* [in] */ BSTR name,
    /* [in] */ VARIANT value);


void __RPC_STUB IXMLDOMElement_setAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMElement_removeAttribute_Proxy( 
    IXMLDOMElement * This,
    /* [in] */ BSTR name);


void __RPC_STUB IXMLDOMElement_removeAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMElement_getAttributeNode_Proxy( 
    IXMLDOMElement * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IXMLDOMAttribute **attributeNode);


void __RPC_STUB IXMLDOMElement_getAttributeNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMElement_setAttributeNode_Proxy( 
    IXMLDOMElement * This,
    /* [in] */ IXMLDOMAttribute *DOMAttribute,
    /* [retval][out] */ IXMLDOMAttribute **attributeNode);


void __RPC_STUB IXMLDOMElement_setAttributeNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMElement_removeAttributeNode_Proxy( 
    IXMLDOMElement * This,
    /* [in] */ IXMLDOMAttribute *DOMAttribute,
    /* [retval][out] */ IXMLDOMAttribute **attributeNode);


void __RPC_STUB IXMLDOMElement_removeAttributeNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMElement_getElementsByTagName_Proxy( 
    IXMLDOMElement * This,
    /* [in] */ BSTR tagName,
    /* [retval][out] */ IXMLDOMNodeList **resultList);


void __RPC_STUB IXMLDOMElement_getElementsByTagName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMElement_normalize_Proxy( 
    IXMLDOMElement * This);


void __RPC_STUB IXMLDOMElement_normalize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMElement_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMText_INTERFACE_DEFINED__
#define __IXMLDOMText_INTERFACE_DEFINED__

/* interface IXMLDOMText */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMText;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF87-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMText : public IXMLDOMCharacterData
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE splitText( 
            /* [in] */ long offset,
            /* [retval][out] */ IXMLDOMText **rightHandTextNode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMTextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMText * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMText * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMText * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMText * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMText * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMText * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMText * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMText * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMText * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMText * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMText * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMText * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMText * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMText * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMText * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMText * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMText * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMText * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMText * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMText * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMText * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMText * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMText * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMText * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMText * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMText * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMText * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMText * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMText * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMText * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMText * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMText * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMText * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMText * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMText * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMText * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMText * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMText * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMText * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMText * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMText * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMText * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMText * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_data )( 
            IXMLDOMText * This,
            /* [retval][out] */ BSTR *data);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_data )( 
            IXMLDOMText * This,
            /* [in] */ BSTR data);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMText * This,
            /* [retval][out] */ long *dataLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *substringData )( 
            IXMLDOMText * This,
            /* [in] */ long offset,
            /* [in] */ long count,
            /* [retval][out] */ BSTR *data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendData )( 
            IXMLDOMText * This,
            /* [in] */ BSTR data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertData )( 
            IXMLDOMText * This,
            /* [in] */ long offset,
            /* [in] */ BSTR data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *deleteData )( 
            IXMLDOMText * This,
            /* [in] */ long offset,
            /* [in] */ long count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceData )( 
            IXMLDOMText * This,
            /* [in] */ long offset,
            /* [in] */ long count,
            /* [in] */ BSTR data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *splitText )( 
            IXMLDOMText * This,
            /* [in] */ long offset,
            /* [retval][out] */ IXMLDOMText **rightHandTextNode);
        
        END_INTERFACE
    } IXMLDOMTextVtbl;

    interface IXMLDOMText
    {
        CONST_VTBL struct IXMLDOMTextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMText_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMText_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMText_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMText_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMText_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMText_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMText_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMText_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMText_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMText_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMText_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMText_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMText_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMText_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMText_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMText_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMText_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMText_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMText_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMText_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMText_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMText_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMText_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMText_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMText_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMText_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMText_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMText_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMText_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMText_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMText_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMText_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMText_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMText_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMText_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMText_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMText_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMText_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMText_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMText_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMText_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMText_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMText_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMText_get_data(This,data)	\
    (This)->lpVtbl -> get_data(This,data)

#define IXMLDOMText_put_data(This,data)	\
    (This)->lpVtbl -> put_data(This,data)

#define IXMLDOMText_get_length(This,dataLength)	\
    (This)->lpVtbl -> get_length(This,dataLength)

#define IXMLDOMText_substringData(This,offset,count,data)	\
    (This)->lpVtbl -> substringData(This,offset,count,data)

#define IXMLDOMText_appendData(This,data)	\
    (This)->lpVtbl -> appendData(This,data)

#define IXMLDOMText_insertData(This,offset,data)	\
    (This)->lpVtbl -> insertData(This,offset,data)

#define IXMLDOMText_deleteData(This,offset,count)	\
    (This)->lpVtbl -> deleteData(This,offset,count)

#define IXMLDOMText_replaceData(This,offset,count,data)	\
    (This)->lpVtbl -> replaceData(This,offset,count,data)


#define IXMLDOMText_splitText(This,offset,rightHandTextNode)	\
    (This)->lpVtbl -> splitText(This,offset,rightHandTextNode)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDOMText_splitText_Proxy( 
    IXMLDOMText * This,
    /* [in] */ long offset,
    /* [retval][out] */ IXMLDOMText **rightHandTextNode);


void __RPC_STUB IXMLDOMText_splitText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMText_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMComment_INTERFACE_DEFINED__
#define __IXMLDOMComment_INTERFACE_DEFINED__

/* interface IXMLDOMComment */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMComment;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF88-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMComment : public IXMLDOMCharacterData
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMCommentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMComment * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMComment * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMComment * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMComment * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMComment * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMComment * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMComment * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMComment * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMComment * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMComment * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMComment * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMComment * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMComment * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMComment * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMComment * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMComment * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMComment * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMComment * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMComment * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMComment * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMComment * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMComment * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMComment * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMComment * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMComment * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMComment * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMComment * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMComment * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMComment * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMComment * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMComment * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMComment * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMComment * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMComment * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMComment * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMComment * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMComment * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMComment * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMComment * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMComment * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMComment * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMComment * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMComment * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_data )( 
            IXMLDOMComment * This,
            /* [retval][out] */ BSTR *data);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_data )( 
            IXMLDOMComment * This,
            /* [in] */ BSTR data);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMComment * This,
            /* [retval][out] */ long *dataLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *substringData )( 
            IXMLDOMComment * This,
            /* [in] */ long offset,
            /* [in] */ long count,
            /* [retval][out] */ BSTR *data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendData )( 
            IXMLDOMComment * This,
            /* [in] */ BSTR data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertData )( 
            IXMLDOMComment * This,
            /* [in] */ long offset,
            /* [in] */ BSTR data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *deleteData )( 
            IXMLDOMComment * This,
            /* [in] */ long offset,
            /* [in] */ long count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceData )( 
            IXMLDOMComment * This,
            /* [in] */ long offset,
            /* [in] */ long count,
            /* [in] */ BSTR data);
        
        END_INTERFACE
    } IXMLDOMCommentVtbl;

    interface IXMLDOMComment
    {
        CONST_VTBL struct IXMLDOMCommentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMComment_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMComment_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMComment_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMComment_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMComment_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMComment_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMComment_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMComment_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMComment_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMComment_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMComment_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMComment_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMComment_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMComment_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMComment_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMComment_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMComment_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMComment_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMComment_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMComment_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMComment_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMComment_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMComment_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMComment_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMComment_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMComment_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMComment_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMComment_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMComment_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMComment_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMComment_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMComment_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMComment_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMComment_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMComment_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMComment_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMComment_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMComment_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMComment_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMComment_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMComment_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMComment_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMComment_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMComment_get_data(This,data)	\
    (This)->lpVtbl -> get_data(This,data)

#define IXMLDOMComment_put_data(This,data)	\
    (This)->lpVtbl -> put_data(This,data)

#define IXMLDOMComment_get_length(This,dataLength)	\
    (This)->lpVtbl -> get_length(This,dataLength)

#define IXMLDOMComment_substringData(This,offset,count,data)	\
    (This)->lpVtbl -> substringData(This,offset,count,data)

#define IXMLDOMComment_appendData(This,data)	\
    (This)->lpVtbl -> appendData(This,data)

#define IXMLDOMComment_insertData(This,offset,data)	\
    (This)->lpVtbl -> insertData(This,offset,data)

#define IXMLDOMComment_deleteData(This,offset,count)	\
    (This)->lpVtbl -> deleteData(This,offset,count)

#define IXMLDOMComment_replaceData(This,offset,count,data)	\
    (This)->lpVtbl -> replaceData(This,offset,count,data)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IXMLDOMComment_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMProcessingInstruction_INTERFACE_DEFINED__
#define __IXMLDOMProcessingInstruction_INTERFACE_DEFINED__

/* interface IXMLDOMProcessingInstruction */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMProcessingInstruction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF89-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMProcessingInstruction : public IXMLDOMNode
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_target( 
            /* [retval][out] */ BSTR *name) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_data( 
            /* [retval][out] */ BSTR *value) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_data( 
            /* [in] */ BSTR value) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMProcessingInstructionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMProcessingInstruction * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMProcessingInstruction * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMProcessingInstruction * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMProcessingInstruction * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMProcessingInstruction * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMProcessingInstruction * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMProcessingInstruction * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMProcessingInstruction * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMProcessingInstruction * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMProcessingInstruction * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMProcessingInstruction * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMProcessingInstruction * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMProcessingInstruction * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_target )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_data )( 
            IXMLDOMProcessingInstruction * This,
            /* [retval][out] */ BSTR *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_data )( 
            IXMLDOMProcessingInstruction * This,
            /* [in] */ BSTR value);
        
        END_INTERFACE
    } IXMLDOMProcessingInstructionVtbl;

    interface IXMLDOMProcessingInstruction
    {
        CONST_VTBL struct IXMLDOMProcessingInstructionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMProcessingInstruction_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMProcessingInstruction_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMProcessingInstruction_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMProcessingInstruction_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMProcessingInstruction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMProcessingInstruction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMProcessingInstruction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMProcessingInstruction_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMProcessingInstruction_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMProcessingInstruction_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMProcessingInstruction_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMProcessingInstruction_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMProcessingInstruction_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMProcessingInstruction_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMProcessingInstruction_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMProcessingInstruction_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMProcessingInstruction_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMProcessingInstruction_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMProcessingInstruction_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMProcessingInstruction_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMProcessingInstruction_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMProcessingInstruction_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMProcessingInstruction_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMProcessingInstruction_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMProcessingInstruction_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMProcessingInstruction_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMProcessingInstruction_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMProcessingInstruction_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMProcessingInstruction_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMProcessingInstruction_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMProcessingInstruction_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMProcessingInstruction_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMProcessingInstruction_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMProcessingInstruction_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMProcessingInstruction_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMProcessingInstruction_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMProcessingInstruction_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMProcessingInstruction_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMProcessingInstruction_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMProcessingInstruction_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMProcessingInstruction_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMProcessingInstruction_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMProcessingInstruction_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMProcessingInstruction_get_target(This,name)	\
    (This)->lpVtbl -> get_target(This,name)

#define IXMLDOMProcessingInstruction_get_data(This,value)	\
    (This)->lpVtbl -> get_data(This,value)

#define IXMLDOMProcessingInstruction_put_data(This,value)	\
    (This)->lpVtbl -> put_data(This,value)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMProcessingInstruction_get_target_Proxy( 
    IXMLDOMProcessingInstruction * This,
    /* [retval][out] */ BSTR *name);


void __RPC_STUB IXMLDOMProcessingInstruction_get_target_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMProcessingInstruction_get_data_Proxy( 
    IXMLDOMProcessingInstruction * This,
    /* [retval][out] */ BSTR *value);


void __RPC_STUB IXMLDOMProcessingInstruction_get_data_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDOMProcessingInstruction_put_data_Proxy( 
    IXMLDOMProcessingInstruction * This,
    /* [in] */ BSTR value);


void __RPC_STUB IXMLDOMProcessingInstruction_put_data_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMProcessingInstruction_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMCDATASection_INTERFACE_DEFINED__
#define __IXMLDOMCDATASection_INTERFACE_DEFINED__

/* interface IXMLDOMCDATASection */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMCDATASection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF8A-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMCDATASection : public IXMLDOMText
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMCDATASectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMCDATASection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMCDATASection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMCDATASection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMCDATASection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMCDATASection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMCDATASection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMCDATASection * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMCDATASection * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMCDATASection * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMCDATASection * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMCDATASection * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMCDATASection * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMCDATASection * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMCDATASection * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMCDATASection * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMCDATASection * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMCDATASection * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMCDATASection * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMCDATASection * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMCDATASection * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMCDATASection * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMCDATASection * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMCDATASection * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMCDATASection * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMCDATASection * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMCDATASection * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMCDATASection * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMCDATASection * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMCDATASection * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMCDATASection * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_data )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ BSTR *data);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_data )( 
            IXMLDOMCDATASection * This,
            /* [in] */ BSTR data);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMCDATASection * This,
            /* [retval][out] */ long *dataLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *substringData )( 
            IXMLDOMCDATASection * This,
            /* [in] */ long offset,
            /* [in] */ long count,
            /* [retval][out] */ BSTR *data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendData )( 
            IXMLDOMCDATASection * This,
            /* [in] */ BSTR data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertData )( 
            IXMLDOMCDATASection * This,
            /* [in] */ long offset,
            /* [in] */ BSTR data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *deleteData )( 
            IXMLDOMCDATASection * This,
            /* [in] */ long offset,
            /* [in] */ long count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceData )( 
            IXMLDOMCDATASection * This,
            /* [in] */ long offset,
            /* [in] */ long count,
            /* [in] */ BSTR data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *splitText )( 
            IXMLDOMCDATASection * This,
            /* [in] */ long offset,
            /* [retval][out] */ IXMLDOMText **rightHandTextNode);
        
        END_INTERFACE
    } IXMLDOMCDATASectionVtbl;

    interface IXMLDOMCDATASection
    {
        CONST_VTBL struct IXMLDOMCDATASectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMCDATASection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMCDATASection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMCDATASection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMCDATASection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMCDATASection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMCDATASection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMCDATASection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMCDATASection_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMCDATASection_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMCDATASection_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMCDATASection_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMCDATASection_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMCDATASection_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMCDATASection_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMCDATASection_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMCDATASection_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMCDATASection_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMCDATASection_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMCDATASection_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMCDATASection_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMCDATASection_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMCDATASection_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMCDATASection_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMCDATASection_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMCDATASection_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMCDATASection_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMCDATASection_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMCDATASection_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMCDATASection_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMCDATASection_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMCDATASection_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMCDATASection_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMCDATASection_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMCDATASection_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMCDATASection_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMCDATASection_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMCDATASection_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMCDATASection_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMCDATASection_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMCDATASection_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMCDATASection_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMCDATASection_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMCDATASection_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMCDATASection_get_data(This,data)	\
    (This)->lpVtbl -> get_data(This,data)

#define IXMLDOMCDATASection_put_data(This,data)	\
    (This)->lpVtbl -> put_data(This,data)

#define IXMLDOMCDATASection_get_length(This,dataLength)	\
    (This)->lpVtbl -> get_length(This,dataLength)

#define IXMLDOMCDATASection_substringData(This,offset,count,data)	\
    (This)->lpVtbl -> substringData(This,offset,count,data)

#define IXMLDOMCDATASection_appendData(This,data)	\
    (This)->lpVtbl -> appendData(This,data)

#define IXMLDOMCDATASection_insertData(This,offset,data)	\
    (This)->lpVtbl -> insertData(This,offset,data)

#define IXMLDOMCDATASection_deleteData(This,offset,count)	\
    (This)->lpVtbl -> deleteData(This,offset,count)

#define IXMLDOMCDATASection_replaceData(This,offset,count,data)	\
    (This)->lpVtbl -> replaceData(This,offset,count,data)


#define IXMLDOMCDATASection_splitText(This,offset,rightHandTextNode)	\
    (This)->lpVtbl -> splitText(This,offset,rightHandTextNode)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IXMLDOMCDATASection_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMDocumentType_INTERFACE_DEFINED__
#define __IXMLDOMDocumentType_INTERFACE_DEFINED__

/* interface IXMLDOMDocumentType */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMDocumentType;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF8B-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMDocumentType : public IXMLDOMNode
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_name( 
            /* [retval][out] */ BSTR *rootName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_entities( 
            /* [retval][out] */ IXMLDOMNamedNodeMap **entityMap) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_notations( 
            /* [retval][out] */ IXMLDOMNamedNodeMap **notationMap) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMDocumentTypeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMDocumentType * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMDocumentType * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMDocumentType * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMDocumentType * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMDocumentType * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMDocumentType * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMDocumentType * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMDocumentType * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMDocumentType * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMDocumentType * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMDocumentType * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMDocumentType * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMDocumentType * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMDocumentType * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMDocumentType * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMDocumentType * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMDocumentType * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMDocumentType * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMDocumentType * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMDocumentType * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMDocumentType * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMDocumentType * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMDocumentType * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMDocumentType * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMDocumentType * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMDocumentType * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMDocumentType * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMDocumentType * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMDocumentType * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMDocumentType * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_name )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ BSTR *rootName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_entities )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **entityMap);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_notations )( 
            IXMLDOMDocumentType * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **notationMap);
        
        END_INTERFACE
    } IXMLDOMDocumentTypeVtbl;

    interface IXMLDOMDocumentType
    {
        CONST_VTBL struct IXMLDOMDocumentTypeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMDocumentType_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMDocumentType_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMDocumentType_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMDocumentType_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMDocumentType_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMDocumentType_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMDocumentType_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMDocumentType_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMDocumentType_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMDocumentType_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMDocumentType_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMDocumentType_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMDocumentType_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMDocumentType_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMDocumentType_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMDocumentType_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMDocumentType_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMDocumentType_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMDocumentType_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMDocumentType_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMDocumentType_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMDocumentType_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMDocumentType_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMDocumentType_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMDocumentType_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMDocumentType_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMDocumentType_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMDocumentType_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMDocumentType_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMDocumentType_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMDocumentType_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMDocumentType_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMDocumentType_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMDocumentType_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMDocumentType_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMDocumentType_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMDocumentType_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMDocumentType_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMDocumentType_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMDocumentType_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMDocumentType_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMDocumentType_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMDocumentType_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMDocumentType_get_name(This,rootName)	\
    (This)->lpVtbl -> get_name(This,rootName)

#define IXMLDOMDocumentType_get_entities(This,entityMap)	\
    (This)->lpVtbl -> get_entities(This,entityMap)

#define IXMLDOMDocumentType_get_notations(This,notationMap)	\
    (This)->lpVtbl -> get_notations(This,notationMap)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocumentType_get_name_Proxy( 
    IXMLDOMDocumentType * This,
    /* [retval][out] */ BSTR *rootName);


void __RPC_STUB IXMLDOMDocumentType_get_name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocumentType_get_entities_Proxy( 
    IXMLDOMDocumentType * This,
    /* [retval][out] */ IXMLDOMNamedNodeMap **entityMap);


void __RPC_STUB IXMLDOMDocumentType_get_entities_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMDocumentType_get_notations_Proxy( 
    IXMLDOMDocumentType * This,
    /* [retval][out] */ IXMLDOMNamedNodeMap **notationMap);


void __RPC_STUB IXMLDOMDocumentType_get_notations_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMDocumentType_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMNotation_INTERFACE_DEFINED__
#define __IXMLDOMNotation_INTERFACE_DEFINED__

/* interface IXMLDOMNotation */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMNotation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF8C-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMNotation : public IXMLDOMNode
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_publicId( 
            /* [retval][out] */ VARIANT *publicID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_systemId( 
            /* [retval][out] */ VARIANT *systemID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMNotationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMNotation * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMNotation * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMNotation * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMNotation * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMNotation * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMNotation * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMNotation * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMNotation * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMNotation * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMNotation * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMNotation * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMNotation * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMNotation * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMNotation * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMNotation * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMNotation * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMNotation * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMNotation * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMNotation * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMNotation * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMNotation * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMNotation * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMNotation * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMNotation * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMNotation * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMNotation * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMNotation * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMNotation * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMNotation * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMNotation * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_publicId )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ VARIANT *publicID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_systemId )( 
            IXMLDOMNotation * This,
            /* [retval][out] */ VARIANT *systemID);
        
        END_INTERFACE
    } IXMLDOMNotationVtbl;

    interface IXMLDOMNotation
    {
        CONST_VTBL struct IXMLDOMNotationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMNotation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMNotation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMNotation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMNotation_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMNotation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMNotation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMNotation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMNotation_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMNotation_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMNotation_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMNotation_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMNotation_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMNotation_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMNotation_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMNotation_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMNotation_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMNotation_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMNotation_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMNotation_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMNotation_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMNotation_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMNotation_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMNotation_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMNotation_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMNotation_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMNotation_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMNotation_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMNotation_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMNotation_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMNotation_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMNotation_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMNotation_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMNotation_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMNotation_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMNotation_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMNotation_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMNotation_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMNotation_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMNotation_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMNotation_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMNotation_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMNotation_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMNotation_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMNotation_get_publicId(This,publicID)	\
    (This)->lpVtbl -> get_publicId(This,publicID)

#define IXMLDOMNotation_get_systemId(This,systemID)	\
    (This)->lpVtbl -> get_systemId(This,systemID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNotation_get_publicId_Proxy( 
    IXMLDOMNotation * This,
    /* [retval][out] */ VARIANT *publicID);


void __RPC_STUB IXMLDOMNotation_get_publicId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMNotation_get_systemId_Proxy( 
    IXMLDOMNotation * This,
    /* [retval][out] */ VARIANT *systemID);


void __RPC_STUB IXMLDOMNotation_get_systemId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMNotation_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMEntity_INTERFACE_DEFINED__
#define __IXMLDOMEntity_INTERFACE_DEFINED__

/* interface IXMLDOMEntity */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMEntity;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF8D-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMEntity : public IXMLDOMNode
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_publicId( 
            /* [retval][out] */ VARIANT *publicID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_systemId( 
            /* [retval][out] */ VARIANT *systemID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_notationName( 
            /* [retval][out] */ BSTR *name) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMEntityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMEntity * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMEntity * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMEntity * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMEntity * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMEntity * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMEntity * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMEntity * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMEntity * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMEntity * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMEntity * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMEntity * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMEntity * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMEntity * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMEntity * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMEntity * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMEntity * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMEntity * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMEntity * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMEntity * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMEntity * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMEntity * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMEntity * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMEntity * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMEntity * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMEntity * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMEntity * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMEntity * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMEntity * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMEntity * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMEntity * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_publicId )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ VARIANT *publicID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_systemId )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ VARIANT *systemID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_notationName )( 
            IXMLDOMEntity * This,
            /* [retval][out] */ BSTR *name);
        
        END_INTERFACE
    } IXMLDOMEntityVtbl;

    interface IXMLDOMEntity
    {
        CONST_VTBL struct IXMLDOMEntityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMEntity_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMEntity_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMEntity_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMEntity_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMEntity_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMEntity_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMEntity_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMEntity_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMEntity_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMEntity_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMEntity_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMEntity_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMEntity_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMEntity_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMEntity_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMEntity_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMEntity_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMEntity_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMEntity_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMEntity_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMEntity_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMEntity_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMEntity_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMEntity_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMEntity_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMEntity_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMEntity_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMEntity_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMEntity_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMEntity_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMEntity_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMEntity_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMEntity_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMEntity_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMEntity_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMEntity_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMEntity_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMEntity_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMEntity_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMEntity_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMEntity_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMEntity_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMEntity_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXMLDOMEntity_get_publicId(This,publicID)	\
    (This)->lpVtbl -> get_publicId(This,publicID)

#define IXMLDOMEntity_get_systemId(This,systemID)	\
    (This)->lpVtbl -> get_systemId(This,systemID)

#define IXMLDOMEntity_get_notationName(This,name)	\
    (This)->lpVtbl -> get_notationName(This,name)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMEntity_get_publicId_Proxy( 
    IXMLDOMEntity * This,
    /* [retval][out] */ VARIANT *publicID);


void __RPC_STUB IXMLDOMEntity_get_publicId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMEntity_get_systemId_Proxy( 
    IXMLDOMEntity * This,
    /* [retval][out] */ VARIANT *systemID);


void __RPC_STUB IXMLDOMEntity_get_systemId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMEntity_get_notationName_Proxy( 
    IXMLDOMEntity * This,
    /* [retval][out] */ BSTR *name);


void __RPC_STUB IXMLDOMEntity_get_notationName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMEntity_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMEntityReference_INTERFACE_DEFINED__
#define __IXMLDOMEntityReference_INTERFACE_DEFINED__

/* interface IXMLDOMEntityReference */
/* [unique][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMEntityReference;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2933BF8E-7B36-11d2-B20E-00C04F983E60")
    IXMLDOMEntityReference : public IXMLDOMNode
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMEntityReferenceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMEntityReference * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMEntityReference * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMEntityReference * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMEntityReference * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMEntityReference * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMEntityReference * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMEntityReference * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMEntityReference * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMEntityReference * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMEntityReference * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMEntityReference * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMEntityReference * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMEntityReference * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMEntityReference * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMEntityReference * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMEntityReference * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMEntityReference * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMEntityReference * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMEntityReference * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMEntityReference * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMEntityReference * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMEntityReference * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMEntityReference * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMEntityReference * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMEntityReference * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMEntityReference * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMEntityReference * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMEntityReference * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMEntityReference * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMEntityReference * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMEntityReference * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        END_INTERFACE
    } IXMLDOMEntityReferenceVtbl;

    interface IXMLDOMEntityReference
    {
        CONST_VTBL struct IXMLDOMEntityReferenceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMEntityReference_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMEntityReference_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMEntityReference_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMEntityReference_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMEntityReference_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMEntityReference_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMEntityReference_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMEntityReference_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXMLDOMEntityReference_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXMLDOMEntityReference_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXMLDOMEntityReference_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXMLDOMEntityReference_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXMLDOMEntityReference_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXMLDOMEntityReference_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXMLDOMEntityReference_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXMLDOMEntityReference_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXMLDOMEntityReference_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXMLDOMEntityReference_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXMLDOMEntityReference_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXMLDOMEntityReference_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXMLDOMEntityReference_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXMLDOMEntityReference_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXMLDOMEntityReference_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXMLDOMEntityReference_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXMLDOMEntityReference_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXMLDOMEntityReference_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXMLDOMEntityReference_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXMLDOMEntityReference_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXMLDOMEntityReference_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXMLDOMEntityReference_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXMLDOMEntityReference_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXMLDOMEntityReference_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXMLDOMEntityReference_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXMLDOMEntityReference_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXMLDOMEntityReference_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXMLDOMEntityReference_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXMLDOMEntityReference_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXMLDOMEntityReference_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXMLDOMEntityReference_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXMLDOMEntityReference_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXMLDOMEntityReference_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXMLDOMEntityReference_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXMLDOMEntityReference_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IXMLDOMEntityReference_INTERFACE_DEFINED__ */


#ifndef __IXMLDOMParseError_INTERFACE_DEFINED__
#define __IXMLDOMParseError_INTERFACE_DEFINED__

/* interface IXMLDOMParseError */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDOMParseError;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3efaa426-272f-11d2-836f-0000f87a7782")
    IXMLDOMParseError : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_errorCode( 
            /* [out][retval] */ long *errorCode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_url( 
            /* [out][retval] */ BSTR *urlString) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_reason( 
            /* [out][retval] */ BSTR *reasonString) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_srcText( 
            /* [out][retval] */ BSTR *sourceString) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_line( 
            /* [out][retval] */ long *lineNumber) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_linepos( 
            /* [out][retval] */ long *linePosition) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_filepos( 
            /* [out][retval] */ long *filePosition) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDOMParseErrorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMParseError * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMParseError * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMParseError * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMParseError * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMParseError * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMParseError * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMParseError * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_errorCode )( 
            IXMLDOMParseError * This,
            /* [out][retval] */ long *errorCode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_url )( 
            IXMLDOMParseError * This,
            /* [out][retval] */ BSTR *urlString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_reason )( 
            IXMLDOMParseError * This,
            /* [out][retval] */ BSTR *reasonString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_srcText )( 
            IXMLDOMParseError * This,
            /* [out][retval] */ BSTR *sourceString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_line )( 
            IXMLDOMParseError * This,
            /* [out][retval] */ long *lineNumber);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_linepos )( 
            IXMLDOMParseError * This,
            /* [out][retval] */ long *linePosition);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_filepos )( 
            IXMLDOMParseError * This,
            /* [out][retval] */ long *filePosition);
        
        END_INTERFACE
    } IXMLDOMParseErrorVtbl;

    interface IXMLDOMParseError
    {
        CONST_VTBL struct IXMLDOMParseErrorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDOMParseError_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDOMParseError_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDOMParseError_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDOMParseError_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDOMParseError_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDOMParseError_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDOMParseError_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDOMParseError_get_errorCode(This,errorCode)	\
    (This)->lpVtbl -> get_errorCode(This,errorCode)

#define IXMLDOMParseError_get_url(This,urlString)	\
    (This)->lpVtbl -> get_url(This,urlString)

#define IXMLDOMParseError_get_reason(This,reasonString)	\
    (This)->lpVtbl -> get_reason(This,reasonString)

#define IXMLDOMParseError_get_srcText(This,sourceString)	\
    (This)->lpVtbl -> get_srcText(This,sourceString)

#define IXMLDOMParseError_get_line(This,lineNumber)	\
    (This)->lpVtbl -> get_line(This,lineNumber)

#define IXMLDOMParseError_get_linepos(This,linePosition)	\
    (This)->lpVtbl -> get_linepos(This,linePosition)

#define IXMLDOMParseError_get_filepos(This,filePosition)	\
    (This)->lpVtbl -> get_filepos(This,filePosition)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMParseError_get_errorCode_Proxy( 
    IXMLDOMParseError * This,
    /* [out][retval] */ long *errorCode);


void __RPC_STUB IXMLDOMParseError_get_errorCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMParseError_get_url_Proxy( 
    IXMLDOMParseError * This,
    /* [out][retval] */ BSTR *urlString);


void __RPC_STUB IXMLDOMParseError_get_url_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMParseError_get_reason_Proxy( 
    IXMLDOMParseError * This,
    /* [out][retval] */ BSTR *reasonString);


void __RPC_STUB IXMLDOMParseError_get_reason_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMParseError_get_srcText_Proxy( 
    IXMLDOMParseError * This,
    /* [out][retval] */ BSTR *sourceString);


void __RPC_STUB IXMLDOMParseError_get_srcText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMParseError_get_line_Proxy( 
    IXMLDOMParseError * This,
    /* [out][retval] */ long *lineNumber);


void __RPC_STUB IXMLDOMParseError_get_line_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMParseError_get_linepos_Proxy( 
    IXMLDOMParseError * This,
    /* [out][retval] */ long *linePosition);


void __RPC_STUB IXMLDOMParseError_get_linepos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDOMParseError_get_filepos_Proxy( 
    IXMLDOMParseError * This,
    /* [out][retval] */ long *filePosition);


void __RPC_STUB IXMLDOMParseError_get_filepos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDOMParseError_INTERFACE_DEFINED__ */


#ifndef __IXTLRuntime_INTERFACE_DEFINED__
#define __IXTLRuntime_INTERFACE_DEFINED__

/* interface IXTLRuntime */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXTLRuntime;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3efaa425-272f-11d2-836f-0000f87a7782")
    IXTLRuntime : public IXMLDOMNode
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE uniqueID( 
            /* [in] */ IXMLDOMNode *pNode,
            /* [retval][out] */ long *pID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE depth( 
            /* [in] */ IXMLDOMNode *pNode,
            /* [retval][out] */ long *pDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE childNumber( 
            /* [in] */ IXMLDOMNode *pNode,
            /* [retval][out] */ long *pNumber) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ancestorChildNumber( 
            /* [in] */ BSTR bstrNodeName,
            /* [in] */ IXMLDOMNode *pNode,
            /* [retval][out] */ long *pNumber) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE absoluteChildNumber( 
            /* [in] */ IXMLDOMNode *pNode,
            /* [retval][out] */ long *pNumber) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE formatIndex( 
            /* [in] */ long lIndex,
            /* [in] */ BSTR bstrFormat,
            /* [retval][out] */ BSTR *pbstrFormattedString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE formatNumber( 
            /* [in] */ double dblNumber,
            /* [in] */ BSTR bstrFormat,
            /* [retval][out] */ BSTR *pbstrFormattedString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE formatDate( 
            /* [in] */ VARIANT varDate,
            /* [in] */ BSTR bstrFormat,
            /* [optional][in] */ VARIANT varDestLocale,
            /* [retval][out] */ BSTR *pbstrFormattedString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE formatTime( 
            /* [in] */ VARIANT varTime,
            /* [in] */ BSTR bstrFormat,
            /* [optional][in] */ VARIANT varDestLocale,
            /* [retval][out] */ BSTR *pbstrFormattedString) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXTLRuntimeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXTLRuntime * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXTLRuntime * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXTLRuntime * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXTLRuntime * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXTLRuntime * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXTLRuntime * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXTLRuntime * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXTLRuntime * This,
            /* [retval][out] */ BSTR *name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXTLRuntime * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXTLRuntime * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXTLRuntime * This,
            /* [retval][out] */ DOMNodeType *type);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXTLRuntime * This,
            /* [retval][out] */ IXMLDOMNode **parent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXTLRuntime * This,
            /* [retval][out] */ IXMLDOMNodeList **childList);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXTLRuntime * This,
            /* [retval][out] */ IXMLDOMNode **firstChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXTLRuntime * This,
            /* [retval][out] */ IXMLDOMNode **lastChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXTLRuntime * This,
            /* [retval][out] */ IXMLDOMNode **previousSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXTLRuntime * This,
            /* [retval][out] */ IXMLDOMNode **nextSibling);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXTLRuntime * This,
            /* [retval][out] */ IXMLDOMNamedNodeMap **attributeMap);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXTLRuntime * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ VARIANT refChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXTLRuntime * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [in] */ IXMLDOMNode *oldChild,
            /* [retval][out] */ IXMLDOMNode **outOldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXTLRuntime * This,
            /* [in] */ IXMLDOMNode *childNode,
            /* [retval][out] */ IXMLDOMNode **oldChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXTLRuntime * This,
            /* [in] */ IXMLDOMNode *newChild,
            /* [retval][out] */ IXMLDOMNode **outNewChild);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXTLRuntime * This,
            /* [retval][out] */ VARIANT_BOOL *hasChild);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXTLRuntime * This,
            /* [retval][out] */ IXMLDOMDocument **DOMDocument);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXTLRuntime * This,
            /* [in] */ VARIANT_BOOL deep,
            /* [retval][out] */ IXMLDOMNode **cloneRoot);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXTLRuntime * This,
            /* [out][retval] */ BSTR *nodeType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXTLRuntime * This,
            /* [out][retval] */ BSTR *text);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXTLRuntime * This,
            /* [in] */ BSTR text);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXTLRuntime * This,
            /* [retval][out] */ VARIANT_BOOL *isSpecified);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXTLRuntime * This,
            /* [out][retval] */ IXMLDOMNode **definitionNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXTLRuntime * This,
            /* [out][retval] */ VARIANT *typedValue);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXTLRuntime * This,
            /* [in] */ VARIANT typedValue);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXTLRuntime * This,
            /* [out][retval] */ VARIANT *dataTypeName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXTLRuntime * This,
            /* [in] */ BSTR dataTypeName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXTLRuntime * This,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXTLRuntime * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [out][retval] */ BSTR *xmlString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXTLRuntime * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNodeList **resultList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXTLRuntime * This,
            /* [in] */ BSTR queryString,
            /* [out][retval] */ IXMLDOMNode **resultNode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXTLRuntime * This,
            /* [out][retval] */ VARIANT_BOOL *isParsed);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXTLRuntime * This,
            /* [out][retval] */ BSTR *namespaceURI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXTLRuntime * This,
            /* [out][retval] */ BSTR *prefixString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXTLRuntime * This,
            /* [out][retval] */ BSTR *nameString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXTLRuntime * This,
            /* [in] */ IXMLDOMNode *stylesheet,
            /* [in] */ VARIANT outputObject);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *uniqueID )( 
            IXTLRuntime * This,
            /* [in] */ IXMLDOMNode *pNode,
            /* [retval][out] */ long *pID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *depth )( 
            IXTLRuntime * This,
            /* [in] */ IXMLDOMNode *pNode,
            /* [retval][out] */ long *pDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *childNumber )( 
            IXTLRuntime * This,
            /* [in] */ IXMLDOMNode *pNode,
            /* [retval][out] */ long *pNumber);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ancestorChildNumber )( 
            IXTLRuntime * This,
            /* [in] */ BSTR bstrNodeName,
            /* [in] */ IXMLDOMNode *pNode,
            /* [retval][out] */ long *pNumber);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *absoluteChildNumber )( 
            IXTLRuntime * This,
            /* [in] */ IXMLDOMNode *pNode,
            /* [retval][out] */ long *pNumber);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *formatIndex )( 
            IXTLRuntime * This,
            /* [in] */ long lIndex,
            /* [in] */ BSTR bstrFormat,
            /* [retval][out] */ BSTR *pbstrFormattedString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *formatNumber )( 
            IXTLRuntime * This,
            /* [in] */ double dblNumber,
            /* [in] */ BSTR bstrFormat,
            /* [retval][out] */ BSTR *pbstrFormattedString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *formatDate )( 
            IXTLRuntime * This,
            /* [in] */ VARIANT varDate,
            /* [in] */ BSTR bstrFormat,
            /* [optional][in] */ VARIANT varDestLocale,
            /* [retval][out] */ BSTR *pbstrFormattedString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *formatTime )( 
            IXTLRuntime * This,
            /* [in] */ VARIANT varTime,
            /* [in] */ BSTR bstrFormat,
            /* [optional][in] */ VARIANT varDestLocale,
            /* [retval][out] */ BSTR *pbstrFormattedString);
        
        END_INTERFACE
    } IXTLRuntimeVtbl;

    interface IXTLRuntime
    {
        CONST_VTBL struct IXTLRuntimeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXTLRuntime_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXTLRuntime_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXTLRuntime_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXTLRuntime_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXTLRuntime_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXTLRuntime_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXTLRuntime_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXTLRuntime_get_nodeName(This,name)	\
    (This)->lpVtbl -> get_nodeName(This,name)

#define IXTLRuntime_get_nodeValue(This,value)	\
    (This)->lpVtbl -> get_nodeValue(This,value)

#define IXTLRuntime_put_nodeValue(This,value)	\
    (This)->lpVtbl -> put_nodeValue(This,value)

#define IXTLRuntime_get_nodeType(This,type)	\
    (This)->lpVtbl -> get_nodeType(This,type)

#define IXTLRuntime_get_parentNode(This,parent)	\
    (This)->lpVtbl -> get_parentNode(This,parent)

#define IXTLRuntime_get_childNodes(This,childList)	\
    (This)->lpVtbl -> get_childNodes(This,childList)

#define IXTLRuntime_get_firstChild(This,firstChild)	\
    (This)->lpVtbl -> get_firstChild(This,firstChild)

#define IXTLRuntime_get_lastChild(This,lastChild)	\
    (This)->lpVtbl -> get_lastChild(This,lastChild)

#define IXTLRuntime_get_previousSibling(This,previousSibling)	\
    (This)->lpVtbl -> get_previousSibling(This,previousSibling)

#define IXTLRuntime_get_nextSibling(This,nextSibling)	\
    (This)->lpVtbl -> get_nextSibling(This,nextSibling)

#define IXTLRuntime_get_attributes(This,attributeMap)	\
    (This)->lpVtbl -> get_attributes(This,attributeMap)

#define IXTLRuntime_insertBefore(This,newChild,refChild,outNewChild)	\
    (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild)

#define IXTLRuntime_replaceChild(This,newChild,oldChild,outOldChild)	\
    (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild)

#define IXTLRuntime_removeChild(This,childNode,oldChild)	\
    (This)->lpVtbl -> removeChild(This,childNode,oldChild)

#define IXTLRuntime_appendChild(This,newChild,outNewChild)	\
    (This)->lpVtbl -> appendChild(This,newChild,outNewChild)

#define IXTLRuntime_hasChildNodes(This,hasChild)	\
    (This)->lpVtbl -> hasChildNodes(This,hasChild)

#define IXTLRuntime_get_ownerDocument(This,DOMDocument)	\
    (This)->lpVtbl -> get_ownerDocument(This,DOMDocument)

#define IXTLRuntime_cloneNode(This,deep,cloneRoot)	\
    (This)->lpVtbl -> cloneNode(This,deep,cloneRoot)

#define IXTLRuntime_get_nodeTypeString(This,nodeType)	\
    (This)->lpVtbl -> get_nodeTypeString(This,nodeType)

#define IXTLRuntime_get_text(This,text)	\
    (This)->lpVtbl -> get_text(This,text)

#define IXTLRuntime_put_text(This,text)	\
    (This)->lpVtbl -> put_text(This,text)

#define IXTLRuntime_get_specified(This,isSpecified)	\
    (This)->lpVtbl -> get_specified(This,isSpecified)

#define IXTLRuntime_get_definition(This,definitionNode)	\
    (This)->lpVtbl -> get_definition(This,definitionNode)

#define IXTLRuntime_get_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> get_nodeTypedValue(This,typedValue)

#define IXTLRuntime_put_nodeTypedValue(This,typedValue)	\
    (This)->lpVtbl -> put_nodeTypedValue(This,typedValue)

#define IXTLRuntime_get_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> get_dataType(This,dataTypeName)

#define IXTLRuntime_put_dataType(This,dataTypeName)	\
    (This)->lpVtbl -> put_dataType(This,dataTypeName)

#define IXTLRuntime_get_xml(This,xmlString)	\
    (This)->lpVtbl -> get_xml(This,xmlString)

#define IXTLRuntime_transformNode(This,stylesheet,xmlString)	\
    (This)->lpVtbl -> transformNode(This,stylesheet,xmlString)

#define IXTLRuntime_selectNodes(This,queryString,resultList)	\
    (This)->lpVtbl -> selectNodes(This,queryString,resultList)

#define IXTLRuntime_selectSingleNode(This,queryString,resultNode)	\
    (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode)

#define IXTLRuntime_get_parsed(This,isParsed)	\
    (This)->lpVtbl -> get_parsed(This,isParsed)

#define IXTLRuntime_get_namespaceURI(This,namespaceURI)	\
    (This)->lpVtbl -> get_namespaceURI(This,namespaceURI)

#define IXTLRuntime_get_prefix(This,prefixString)	\
    (This)->lpVtbl -> get_prefix(This,prefixString)

#define IXTLRuntime_get_baseName(This,nameString)	\
    (This)->lpVtbl -> get_baseName(This,nameString)

#define IXTLRuntime_transformNodeToObject(This,stylesheet,outputObject)	\
    (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject)


#define IXTLRuntime_uniqueID(This,pNode,pID)	\
    (This)->lpVtbl -> uniqueID(This,pNode,pID)

#define IXTLRuntime_depth(This,pNode,pDepth)	\
    (This)->lpVtbl -> depth(This,pNode,pDepth)

#define IXTLRuntime_childNumber(This,pNode,pNumber)	\
    (This)->lpVtbl -> childNumber(This,pNode,pNumber)

#define IXTLRuntime_ancestorChildNumber(This,bstrNodeName,pNode,pNumber)	\
    (This)->lpVtbl -> ancestorChildNumber(This,bstrNodeName,pNode,pNumber)

#define IXTLRuntime_absoluteChildNumber(This,pNode,pNumber)	\
    (This)->lpVtbl -> absoluteChildNumber(This,pNode,pNumber)

#define IXTLRuntime_formatIndex(This,lIndex,bstrFormat,pbstrFormattedString)	\
    (This)->lpVtbl -> formatIndex(This,lIndex,bstrFormat,pbstrFormattedString)

#define IXTLRuntime_formatNumber(This,dblNumber,bstrFormat,pbstrFormattedString)	\
    (This)->lpVtbl -> formatNumber(This,dblNumber,bstrFormat,pbstrFormattedString)

#define IXTLRuntime_formatDate(This,varDate,bstrFormat,varDestLocale,pbstrFormattedString)	\
    (This)->lpVtbl -> formatDate(This,varDate,bstrFormat,varDestLocale,pbstrFormattedString)

#define IXTLRuntime_formatTime(This,varTime,bstrFormat,varDestLocale,pbstrFormattedString)	\
    (This)->lpVtbl -> formatTime(This,varTime,bstrFormat,varDestLocale,pbstrFormattedString)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXTLRuntime_uniqueID_Proxy( 
    IXTLRuntime * This,
    /* [in] */ IXMLDOMNode *pNode,
    /* [retval][out] */ long *pID);


void __RPC_STUB IXTLRuntime_uniqueID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXTLRuntime_depth_Proxy( 
    IXTLRuntime * This,
    /* [in] */ IXMLDOMNode *pNode,
    /* [retval][out] */ long *pDepth);


void __RPC_STUB IXTLRuntime_depth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXTLRuntime_childNumber_Proxy( 
    IXTLRuntime * This,
    /* [in] */ IXMLDOMNode *pNode,
    /* [retval][out] */ long *pNumber);


void __RPC_STUB IXTLRuntime_childNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXTLRuntime_ancestorChildNumber_Proxy( 
    IXTLRuntime * This,
    /* [in] */ BSTR bstrNodeName,
    /* [in] */ IXMLDOMNode *pNode,
    /* [retval][out] */ long *pNumber);


void __RPC_STUB IXTLRuntime_ancestorChildNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXTLRuntime_absoluteChildNumber_Proxy( 
    IXTLRuntime * This,
    /* [in] */ IXMLDOMNode *pNode,
    /* [retval][out] */ long *pNumber);


void __RPC_STUB IXTLRuntime_absoluteChildNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXTLRuntime_formatIndex_Proxy( 
    IXTLRuntime * This,
    /* [in] */ long lIndex,
    /* [in] */ BSTR bstrFormat,
    /* [retval][out] */ BSTR *pbstrFormattedString);


void __RPC_STUB IXTLRuntime_formatIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXTLRuntime_formatNumber_Proxy( 
    IXTLRuntime * This,
    /* [in] */ double dblNumber,
    /* [in] */ BSTR bstrFormat,
    /* [retval][out] */ BSTR *pbstrFormattedString);


void __RPC_STUB IXTLRuntime_formatNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXTLRuntime_formatDate_Proxy( 
    IXTLRuntime * This,
    /* [in] */ VARIANT varDate,
    /* [in] */ BSTR bstrFormat,
    /* [optional][in] */ VARIANT varDestLocale,
    /* [retval][out] */ BSTR *pbstrFormattedString);


void __RPC_STUB IXTLRuntime_formatDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXTLRuntime_formatTime_Proxy( 
    IXTLRuntime * This,
    /* [in] */ VARIANT varTime,
    /* [in] */ BSTR bstrFormat,
    /* [optional][in] */ VARIANT varDestLocale,
    /* [retval][out] */ BSTR *pbstrFormattedString);


void __RPC_STUB IXTLRuntime_formatTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXTLRuntime_INTERFACE_DEFINED__ */


#ifndef __XMLDOMDocumentEvents_DISPINTERFACE_DEFINED__
#define __XMLDOMDocumentEvents_DISPINTERFACE_DEFINED__

/* dispinterface XMLDOMDocumentEvents */
/* [uuid][hidden] */ 


EXTERN_C const IID DIID_XMLDOMDocumentEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("3efaa427-272f-11d2-836f-0000f87a7782")
    XMLDOMDocumentEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct XMLDOMDocumentEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            XMLDOMDocumentEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            XMLDOMDocumentEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            XMLDOMDocumentEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            XMLDOMDocumentEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            XMLDOMDocumentEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            XMLDOMDocumentEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            XMLDOMDocumentEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } XMLDOMDocumentEventsVtbl;

    interface XMLDOMDocumentEvents
    {
        CONST_VTBL struct XMLDOMDocumentEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define XMLDOMDocumentEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define XMLDOMDocumentEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define XMLDOMDocumentEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define XMLDOMDocumentEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define XMLDOMDocumentEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define XMLDOMDocumentEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define XMLDOMDocumentEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __XMLDOMDocumentEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_DOMDocument;

#ifdef __cplusplus

class DECLSPEC_UUID("2933BF90-7B36-11d2-B20E-00C04F983E60")
DOMDocument;
#endif

EXTERN_C const CLSID CLSID_DOMFreeThreadedDocument;

#ifdef __cplusplus

class DECLSPEC_UUID("2933BF91-7B36-11d2-B20E-00C04F983E60")
DOMFreeThreadedDocument;
#endif

#ifndef __IXMLHttpRequest_INTERFACE_DEFINED__
#define __IXMLHttpRequest_INTERFACE_DEFINED__

/* interface IXMLHttpRequest */
/* [unique][helpstring][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IXMLHttpRequest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ED8C108D-4349-11D2-91A4-00C04F7969E8")
    IXMLHttpRequest : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE open( 
            /* [in] */ BSTR bstrMethod,
            /* [in] */ BSTR bstrUrl,
            /* [optional][in] */ VARIANT varAsync,
            /* [optional][in] */ VARIANT bstrUser,
            /* [optional][in] */ VARIANT bstrPassword) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setRequestHeader( 
            /* [in] */ BSTR bstrHeader,
            /* [in] */ BSTR bstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getResponseHeader( 
            /* [in] */ BSTR bstrHeader,
            /* [retval][out] */ BSTR *pbstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getAllResponseHeaders( 
            /* [retval][out] */ BSTR *pbstrHeaders) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE send( 
            /* [optional][in] */ VARIANT varBody) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE abort( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_status( 
            /* [retval][out] */ long *plStatus) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_statusText( 
            /* [retval][out] */ BSTR *pbstrStatus) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_responseXML( 
            /* [retval][out] */ IDispatch **ppBody) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_responseText( 
            /* [retval][out] */ BSTR *pbstrBody) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_responseBody( 
            /* [retval][out] */ VARIANT *pvarBody) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_responseStream( 
            /* [retval][out] */ VARIANT *pvarBody) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_readyState( 
            /* [retval][out] */ long *plState) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_onreadystatechange( 
            /* [in] */ IDispatch *pReadyStateSink) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLHttpRequestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLHttpRequest * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLHttpRequest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLHttpRequest * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLHttpRequest * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLHttpRequest * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLHttpRequest * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLHttpRequest * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *open )( 
            IXMLHttpRequest * This,
            /* [in] */ BSTR bstrMethod,
            /* [in] */ BSTR bstrUrl,
            /* [optional][in] */ VARIANT varAsync,
            /* [optional][in] */ VARIANT bstrUser,
            /* [optional][in] */ VARIANT bstrPassword);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setRequestHeader )( 
            IXMLHttpRequest * This,
            /* [in] */ BSTR bstrHeader,
            /* [in] */ BSTR bstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getResponseHeader )( 
            IXMLHttpRequest * This,
            /* [in] */ BSTR bstrHeader,
            /* [retval][out] */ BSTR *pbstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getAllResponseHeaders )( 
            IXMLHttpRequest * This,
            /* [retval][out] */ BSTR *pbstrHeaders);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *send )( 
            IXMLHttpRequest * This,
            /* [optional][in] */ VARIANT varBody);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *abort )( 
            IXMLHttpRequest * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_status )( 
            IXMLHttpRequest * This,
            /* [retval][out] */ long *plStatus);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_statusText )( 
            IXMLHttpRequest * This,
            /* [retval][out] */ BSTR *pbstrStatus);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_responseXML )( 
            IXMLHttpRequest * This,
            /* [retval][out] */ IDispatch **ppBody);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_responseText )( 
            IXMLHttpRequest * This,
            /* [retval][out] */ BSTR *pbstrBody);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_responseBody )( 
            IXMLHttpRequest * This,
            /* [retval][out] */ VARIANT *pvarBody);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_responseStream )( 
            IXMLHttpRequest * This,
            /* [retval][out] */ VARIANT *pvarBody);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IXMLHttpRequest * This,
            /* [retval][out] */ long *plState);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_onreadystatechange )( 
            IXMLHttpRequest * This,
            /* [in] */ IDispatch *pReadyStateSink);
        
        END_INTERFACE
    } IXMLHttpRequestVtbl;

    interface IXMLHttpRequest
    {
        CONST_VTBL struct IXMLHttpRequestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLHttpRequest_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLHttpRequest_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLHttpRequest_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLHttpRequest_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLHttpRequest_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLHttpRequest_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLHttpRequest_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLHttpRequest_open(This,bstrMethod,bstrUrl,varAsync,bstrUser,bstrPassword)	\
    (This)->lpVtbl -> open(This,bstrMethod,bstrUrl,varAsync,bstrUser,bstrPassword)

#define IXMLHttpRequest_setRequestHeader(This,bstrHeader,bstrValue)	\
    (This)->lpVtbl -> setRequestHeader(This,bstrHeader,bstrValue)

#define IXMLHttpRequest_getResponseHeader(This,bstrHeader,pbstrValue)	\
    (This)->lpVtbl -> getResponseHeader(This,bstrHeader,pbstrValue)

#define IXMLHttpRequest_getAllResponseHeaders(This,pbstrHeaders)	\
    (This)->lpVtbl -> getAllResponseHeaders(This,pbstrHeaders)

#define IXMLHttpRequest_send(This,varBody)	\
    (This)->lpVtbl -> send(This,varBody)

#define IXMLHttpRequest_abort(This)	\
    (This)->lpVtbl -> abort(This)

#define IXMLHttpRequest_get_status(This,plStatus)	\
    (This)->lpVtbl -> get_status(This,plStatus)

#define IXMLHttpRequest_get_statusText(This,pbstrStatus)	\
    (This)->lpVtbl -> get_statusText(This,pbstrStatus)

#define IXMLHttpRequest_get_responseXML(This,ppBody)	\
    (This)->lpVtbl -> get_responseXML(This,ppBody)

#define IXMLHttpRequest_get_responseText(This,pbstrBody)	\
    (This)->lpVtbl -> get_responseText(This,pbstrBody)

#define IXMLHttpRequest_get_responseBody(This,pvarBody)	\
    (This)->lpVtbl -> get_responseBody(This,pvarBody)

#define IXMLHttpRequest_get_responseStream(This,pvarBody)	\
    (This)->lpVtbl -> get_responseStream(This,pvarBody)

#define IXMLHttpRequest_get_readyState(This,plState)	\
    (This)->lpVtbl -> get_readyState(This,plState)

#define IXMLHttpRequest_put_onreadystatechange(This,pReadyStateSink)	\
    (This)->lpVtbl -> put_onreadystatechange(This,pReadyStateSink)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_open_Proxy( 
    IXMLHttpRequest * This,
    /* [in] */ BSTR bstrMethod,
    /* [in] */ BSTR bstrUrl,
    /* [optional][in] */ VARIANT varAsync,
    /* [optional][in] */ VARIANT bstrUser,
    /* [optional][in] */ VARIANT bstrPassword);


void __RPC_STUB IXMLHttpRequest_open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_setRequestHeader_Proxy( 
    IXMLHttpRequest * This,
    /* [in] */ BSTR bstrHeader,
    /* [in] */ BSTR bstrValue);


void __RPC_STUB IXMLHttpRequest_setRequestHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_getResponseHeader_Proxy( 
    IXMLHttpRequest * This,
    /* [in] */ BSTR bstrHeader,
    /* [retval][out] */ BSTR *pbstrValue);


void __RPC_STUB IXMLHttpRequest_getResponseHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_getAllResponseHeaders_Proxy( 
    IXMLHttpRequest * This,
    /* [retval][out] */ BSTR *pbstrHeaders);


void __RPC_STUB IXMLHttpRequest_getAllResponseHeaders_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_send_Proxy( 
    IXMLHttpRequest * This,
    /* [optional][in] */ VARIANT varBody);


void __RPC_STUB IXMLHttpRequest_send_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_abort_Proxy( 
    IXMLHttpRequest * This);


void __RPC_STUB IXMLHttpRequest_abort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_get_status_Proxy( 
    IXMLHttpRequest * This,
    /* [retval][out] */ long *plStatus);


void __RPC_STUB IXMLHttpRequest_get_status_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_get_statusText_Proxy( 
    IXMLHttpRequest * This,
    /* [retval][out] */ BSTR *pbstrStatus);


void __RPC_STUB IXMLHttpRequest_get_statusText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_get_responseXML_Proxy( 
    IXMLHttpRequest * This,
    /* [retval][out] */ IDispatch **ppBody);


void __RPC_STUB IXMLHttpRequest_get_responseXML_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_get_responseText_Proxy( 
    IXMLHttpRequest * This,
    /* [retval][out] */ BSTR *pbstrBody);


void __RPC_STUB IXMLHttpRequest_get_responseText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_get_responseBody_Proxy( 
    IXMLHttpRequest * This,
    /* [retval][out] */ VARIANT *pvarBody);


void __RPC_STUB IXMLHttpRequest_get_responseBody_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_get_responseStream_Proxy( 
    IXMLHttpRequest * This,
    /* [retval][out] */ VARIANT *pvarBody);


void __RPC_STUB IXMLHttpRequest_get_responseStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_get_readyState_Proxy( 
    IXMLHttpRequest * This,
    /* [retval][out] */ long *plState);


void __RPC_STUB IXMLHttpRequest_get_readyState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLHttpRequest_put_onreadystatechange_Proxy( 
    IXMLHttpRequest * This,
    /* [in] */ IDispatch *pReadyStateSink);


void __RPC_STUB IXMLHttpRequest_put_onreadystatechange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLHttpRequest_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_XMLHTTPRequest;

#ifdef __cplusplus

class DECLSPEC_UUID("ED8C108E-4349-11D2-91A4-00C04F7969E8")
XMLHTTPRequest;
#endif

#ifndef __IXMLDSOControl_INTERFACE_DEFINED__
#define __IXMLDSOControl_INTERFACE_DEFINED__

/* interface IXMLDSOControl */
/* [unique][helpstring][hidden][nonextensible][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDSOControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("310afa62-0575-11d2-9ca9-0060b0ec3d39")
    IXMLDSOControl : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_XMLDocument( 
            /* [retval][out] */ IXMLDOMDocument **ppDoc) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_XMLDocument( 
            /* [in] */ IXMLDOMDocument *ppDoc) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_JavaDSOCompatible( 
            /* [retval][out] */ BOOL *fJavaDSOCompatible) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_JavaDSOCompatible( 
            /* [in] */ BOOL fJavaDSOCompatible) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_readyState( 
            /* [retval][out] */ long *state) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDSOControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDSOControl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDSOControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDSOControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDSOControl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDSOControl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDSOControl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDSOControl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_XMLDocument )( 
            IXMLDSOControl * This,
            /* [retval][out] */ IXMLDOMDocument **ppDoc);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_XMLDocument )( 
            IXMLDSOControl * This,
            /* [in] */ IXMLDOMDocument *ppDoc);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_JavaDSOCompatible )( 
            IXMLDSOControl * This,
            /* [retval][out] */ BOOL *fJavaDSOCompatible);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_JavaDSOCompatible )( 
            IXMLDSOControl * This,
            /* [in] */ BOOL fJavaDSOCompatible);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IXMLDSOControl * This,
            /* [retval][out] */ long *state);
        
        END_INTERFACE
    } IXMLDSOControlVtbl;

    interface IXMLDSOControl
    {
        CONST_VTBL struct IXMLDSOControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDSOControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDSOControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDSOControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDSOControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDSOControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDSOControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDSOControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDSOControl_get_XMLDocument(This,ppDoc)	\
    (This)->lpVtbl -> get_XMLDocument(This,ppDoc)

#define IXMLDSOControl_put_XMLDocument(This,ppDoc)	\
    (This)->lpVtbl -> put_XMLDocument(This,ppDoc)

#define IXMLDSOControl_get_JavaDSOCompatible(This,fJavaDSOCompatible)	\
    (This)->lpVtbl -> get_JavaDSOCompatible(This,fJavaDSOCompatible)

#define IXMLDSOControl_put_JavaDSOCompatible(This,fJavaDSOCompatible)	\
    (This)->lpVtbl -> put_JavaDSOCompatible(This,fJavaDSOCompatible)

#define IXMLDSOControl_get_readyState(This,state)	\
    (This)->lpVtbl -> get_readyState(This,state)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDSOControl_get_XMLDocument_Proxy( 
    IXMLDSOControl * This,
    /* [retval][out] */ IXMLDOMDocument **ppDoc);


void __RPC_STUB IXMLDSOControl_get_XMLDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDSOControl_put_XMLDocument_Proxy( 
    IXMLDSOControl * This,
    /* [in] */ IXMLDOMDocument *ppDoc);


void __RPC_STUB IXMLDSOControl_put_XMLDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDSOControl_get_JavaDSOCompatible_Proxy( 
    IXMLDSOControl * This,
    /* [retval][out] */ BOOL *fJavaDSOCompatible);


void __RPC_STUB IXMLDSOControl_get_JavaDSOCompatible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDSOControl_put_JavaDSOCompatible_Proxy( 
    IXMLDSOControl * This,
    /* [in] */ BOOL fJavaDSOCompatible);


void __RPC_STUB IXMLDSOControl_put_JavaDSOCompatible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDSOControl_get_readyState_Proxy( 
    IXMLDSOControl * This,
    /* [retval][out] */ long *state);


void __RPC_STUB IXMLDSOControl_get_readyState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDSOControl_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_XMLDSOControl;

#ifdef __cplusplus

class DECLSPEC_UUID("550dda30-0541-11d2-9ca9-0060b0ec3d39")
XMLDSOControl;
#endif

#ifndef __IXMLElementCollection_INTERFACE_DEFINED__
#define __IXMLElementCollection_INTERFACE_DEFINED__

/* interface IXMLElementCollection */
/* [helpstring][hidden][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLElementCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("65725580-9B5D-11d0-9BFE-00C04FC99C8E")
    IXMLElementCollection : public IDispatch
    {
    public:
        virtual /* [id][hidden][restricted][propput] */ HRESULT STDMETHODCALLTYPE put_length( 
            /* [in] */ long v) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_length( 
            /* [out][retval] */ long *p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__newEnum( 
            /* [out][retval] */ IUnknown **ppUnk) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE item( 
            /* [in][optional] */ VARIANT var1,
            /* [in][optional] */ VARIANT var2,
            /* [out][retval] */ IDispatch **ppDisp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLElementCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLElementCollection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLElementCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLElementCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLElementCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLElementCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLElementCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLElementCollection * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][hidden][restricted][propput] */ HRESULT ( STDMETHODCALLTYPE *put_length )( 
            IXMLElementCollection * This,
            /* [in] */ long v);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLElementCollection * This,
            /* [out][retval] */ long *p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            IXMLElementCollection * This,
            /* [out][retval] */ IUnknown **ppUnk);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *item )( 
            IXMLElementCollection * This,
            /* [in][optional] */ VARIANT var1,
            /* [in][optional] */ VARIANT var2,
            /* [out][retval] */ IDispatch **ppDisp);
        
        END_INTERFACE
    } IXMLElementCollectionVtbl;

    interface IXMLElementCollection
    {
        CONST_VTBL struct IXMLElementCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLElementCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLElementCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLElementCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLElementCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLElementCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLElementCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLElementCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLElementCollection_put_length(This,v)	\
    (This)->lpVtbl -> put_length(This,v)

#define IXMLElementCollection_get_length(This,p)	\
    (This)->lpVtbl -> get_length(This,p)

#define IXMLElementCollection_get__newEnum(This,ppUnk)	\
    (This)->lpVtbl -> get__newEnum(This,ppUnk)

#define IXMLElementCollection_item(This,var1,var2,ppDisp)	\
    (This)->lpVtbl -> item(This,var1,var2,ppDisp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][hidden][restricted][propput] */ HRESULT STDMETHODCALLTYPE IXMLElementCollection_put_length_Proxy( 
    IXMLElementCollection * This,
    /* [in] */ long v);


void __RPC_STUB IXMLElementCollection_put_length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElementCollection_get_length_Proxy( 
    IXMLElementCollection * This,
    /* [out][retval] */ long *p);


void __RPC_STUB IXMLElementCollection_get_length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLElementCollection_get__newEnum_Proxy( 
    IXMLElementCollection * This,
    /* [out][retval] */ IUnknown **ppUnk);


void __RPC_STUB IXMLElementCollection_get__newEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElementCollection_item_Proxy( 
    IXMLElementCollection * This,
    /* [in][optional] */ VARIANT var1,
    /* [in][optional] */ VARIANT var2,
    /* [out][retval] */ IDispatch **ppDisp);


void __RPC_STUB IXMLElementCollection_item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLElementCollection_INTERFACE_DEFINED__ */


#ifndef __IXMLDocument_INTERFACE_DEFINED__
#define __IXMLDocument_INTERFACE_DEFINED__

/* interface IXMLDocument */
/* [helpstring][hidden][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F52E2B61-18A1-11d1-B105-00805F49916B")
    IXMLDocument : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_root( 
            /* [out][retval] */ IXMLElement **p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_fileSize( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_fileModifiedDate( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_fileUpdatedDate( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_URL( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_URL( 
            /* [in] */ BSTR p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_mimeType( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_readyState( 
            /* [out][retval] */ long *pl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_charset( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_charset( 
            /* [in] */ BSTR p) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_version( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_doctype( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_dtdURL( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createElement( 
            /* [in] */ VARIANT vType,
            /* [in][optional] */ VARIANT var1,
            /* [out][retval] */ IXMLElement **ppElem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDocument * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDocument * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDocument * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDocument * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDocument * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDocument * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDocument * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_root )( 
            IXMLDocument * This,
            /* [out][retval] */ IXMLElement **p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_fileSize )( 
            IXMLDocument * This,
            /* [out][retval] */ BSTR *p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_fileModifiedDate )( 
            IXMLDocument * This,
            /* [out][retval] */ BSTR *p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_fileUpdatedDate )( 
            IXMLDocument * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_URL )( 
            IXMLDocument * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_URL )( 
            IXMLDocument * This,
            /* [in] */ BSTR p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_mimeType )( 
            IXMLDocument * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IXMLDocument * This,
            /* [out][retval] */ long *pl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_charset )( 
            IXMLDocument * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_charset )( 
            IXMLDocument * This,
            /* [in] */ BSTR p);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_version )( 
            IXMLDocument * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_doctype )( 
            IXMLDocument * This,
            /* [out][retval] */ BSTR *p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dtdURL )( 
            IXMLDocument * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createElement )( 
            IXMLDocument * This,
            /* [in] */ VARIANT vType,
            /* [in][optional] */ VARIANT var1,
            /* [out][retval] */ IXMLElement **ppElem);
        
        END_INTERFACE
    } IXMLDocumentVtbl;

    interface IXMLDocument
    {
        CONST_VTBL struct IXMLDocumentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDocument_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDocument_get_root(This,p)	\
    (This)->lpVtbl -> get_root(This,p)

#define IXMLDocument_get_fileSize(This,p)	\
    (This)->lpVtbl -> get_fileSize(This,p)

#define IXMLDocument_get_fileModifiedDate(This,p)	\
    (This)->lpVtbl -> get_fileModifiedDate(This,p)

#define IXMLDocument_get_fileUpdatedDate(This,p)	\
    (This)->lpVtbl -> get_fileUpdatedDate(This,p)

#define IXMLDocument_get_URL(This,p)	\
    (This)->lpVtbl -> get_URL(This,p)

#define IXMLDocument_put_URL(This,p)	\
    (This)->lpVtbl -> put_URL(This,p)

#define IXMLDocument_get_mimeType(This,p)	\
    (This)->lpVtbl -> get_mimeType(This,p)

#define IXMLDocument_get_readyState(This,pl)	\
    (This)->lpVtbl -> get_readyState(This,pl)

#define IXMLDocument_get_charset(This,p)	\
    (This)->lpVtbl -> get_charset(This,p)

#define IXMLDocument_put_charset(This,p)	\
    (This)->lpVtbl -> put_charset(This,p)

#define IXMLDocument_get_version(This,p)	\
    (This)->lpVtbl -> get_version(This,p)

#define IXMLDocument_get_doctype(This,p)	\
    (This)->lpVtbl -> get_doctype(This,p)

#define IXMLDocument_get_dtdURL(This,p)	\
    (This)->lpVtbl -> get_dtdURL(This,p)

#define IXMLDocument_createElement(This,vType,var1,ppElem)	\
    (This)->lpVtbl -> createElement(This,vType,var1,ppElem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_root_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ IXMLElement **p);


void __RPC_STUB IXMLDocument_get_root_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_fileSize_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument_get_fileSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_fileModifiedDate_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument_get_fileModifiedDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_fileUpdatedDate_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument_get_fileUpdatedDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_URL_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument_get_URL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDocument_put_URL_Proxy( 
    IXMLDocument * This,
    /* [in] */ BSTR p);


void __RPC_STUB IXMLDocument_put_URL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_mimeType_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument_get_mimeType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_readyState_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ long *pl);


void __RPC_STUB IXMLDocument_get_readyState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_charset_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument_get_charset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDocument_put_charset_Proxy( 
    IXMLDocument * This,
    /* [in] */ BSTR p);


void __RPC_STUB IXMLDocument_put_charset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_version_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument_get_version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_doctype_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument_get_doctype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument_get_dtdURL_Proxy( 
    IXMLDocument * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument_get_dtdURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDocument_createElement_Proxy( 
    IXMLDocument * This,
    /* [in] */ VARIANT vType,
    /* [in][optional] */ VARIANT var1,
    /* [out][retval] */ IXMLElement **ppElem);


void __RPC_STUB IXMLDocument_createElement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDocument_INTERFACE_DEFINED__ */


#ifndef __IXMLDocument2_INTERFACE_DEFINED__
#define __IXMLDocument2_INTERFACE_DEFINED__

/* interface IXMLDocument2 */
/* [hidden][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLDocument2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2B8DE2FE-8D2D-11d1-B2FC-00C04FD915A9")
    IXMLDocument2 : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_root( 
            /* [out][retval] */ IXMLElement2 **p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_fileSize( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_fileModifiedDate( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_fileUpdatedDate( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_URL( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_URL( 
            /* [in] */ BSTR p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_mimeType( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_readyState( 
            /* [out][retval] */ long *pl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_charset( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_charset( 
            /* [in] */ BSTR p) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_version( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_doctype( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_dtdURL( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createElement( 
            /* [in] */ VARIANT vType,
            /* [in][optional] */ VARIANT var1,
            /* [out][retval] */ IXMLElement2 **ppElem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_async( 
            /* [out][retval] */ VARIANT_BOOL *pf) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_async( 
            /* [in] */ VARIANT_BOOL f) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLDocument2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDocument2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDocument2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDocument2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDocument2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDocument2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDocument2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDocument2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_root )( 
            IXMLDocument2 * This,
            /* [out][retval] */ IXMLElement2 **p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_fileSize )( 
            IXMLDocument2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_fileModifiedDate )( 
            IXMLDocument2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_fileUpdatedDate )( 
            IXMLDocument2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_URL )( 
            IXMLDocument2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_URL )( 
            IXMLDocument2 * This,
            /* [in] */ BSTR p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_mimeType )( 
            IXMLDocument2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IXMLDocument2 * This,
            /* [out][retval] */ long *pl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_charset )( 
            IXMLDocument2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_charset )( 
            IXMLDocument2 * This,
            /* [in] */ BSTR p);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_version )( 
            IXMLDocument2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_doctype )( 
            IXMLDocument2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [id][hidden][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get_dtdURL )( 
            IXMLDocument2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createElement )( 
            IXMLDocument2 * This,
            /* [in] */ VARIANT vType,
            /* [in][optional] */ VARIANT var1,
            /* [out][retval] */ IXMLElement2 **ppElem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_async )( 
            IXMLDocument2 * This,
            /* [out][retval] */ VARIANT_BOOL *pf);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_async )( 
            IXMLDocument2 * This,
            /* [in] */ VARIANT_BOOL f);
        
        END_INTERFACE
    } IXMLDocument2Vtbl;

    interface IXMLDocument2
    {
        CONST_VTBL struct IXMLDocument2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLDocument2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLDocument2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLDocument2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLDocument2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLDocument2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLDocument2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLDocument2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLDocument2_get_root(This,p)	\
    (This)->lpVtbl -> get_root(This,p)

#define IXMLDocument2_get_fileSize(This,p)	\
    (This)->lpVtbl -> get_fileSize(This,p)

#define IXMLDocument2_get_fileModifiedDate(This,p)	\
    (This)->lpVtbl -> get_fileModifiedDate(This,p)

#define IXMLDocument2_get_fileUpdatedDate(This,p)	\
    (This)->lpVtbl -> get_fileUpdatedDate(This,p)

#define IXMLDocument2_get_URL(This,p)	\
    (This)->lpVtbl -> get_URL(This,p)

#define IXMLDocument2_put_URL(This,p)	\
    (This)->lpVtbl -> put_URL(This,p)

#define IXMLDocument2_get_mimeType(This,p)	\
    (This)->lpVtbl -> get_mimeType(This,p)

#define IXMLDocument2_get_readyState(This,pl)	\
    (This)->lpVtbl -> get_readyState(This,pl)

#define IXMLDocument2_get_charset(This,p)	\
    (This)->lpVtbl -> get_charset(This,p)

#define IXMLDocument2_put_charset(This,p)	\
    (This)->lpVtbl -> put_charset(This,p)

#define IXMLDocument2_get_version(This,p)	\
    (This)->lpVtbl -> get_version(This,p)

#define IXMLDocument2_get_doctype(This,p)	\
    (This)->lpVtbl -> get_doctype(This,p)

#define IXMLDocument2_get_dtdURL(This,p)	\
    (This)->lpVtbl -> get_dtdURL(This,p)

#define IXMLDocument2_createElement(This,vType,var1,ppElem)	\
    (This)->lpVtbl -> createElement(This,vType,var1,ppElem)

#define IXMLDocument2_get_async(This,pf)	\
    (This)->lpVtbl -> get_async(This,pf)

#define IXMLDocument2_put_async(This,f)	\
    (This)->lpVtbl -> put_async(This,f)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_root_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ IXMLElement2 **p);


void __RPC_STUB IXMLDocument2_get_root_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_fileSize_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument2_get_fileSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_fileModifiedDate_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument2_get_fileModifiedDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_fileUpdatedDate_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument2_get_fileUpdatedDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_URL_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument2_get_URL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_put_URL_Proxy( 
    IXMLDocument2 * This,
    /* [in] */ BSTR p);


void __RPC_STUB IXMLDocument2_put_URL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_mimeType_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument2_get_mimeType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_readyState_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ long *pl);


void __RPC_STUB IXMLDocument2_get_readyState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_charset_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument2_get_charset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_put_charset_Proxy( 
    IXMLDocument2 * This,
    /* [in] */ BSTR p);


void __RPC_STUB IXMLDocument2_put_charset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_version_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument2_get_version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_doctype_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument2_get_doctype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_dtdURL_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLDocument2_get_dtdURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_createElement_Proxy( 
    IXMLDocument2 * This,
    /* [in] */ VARIANT vType,
    /* [in][optional] */ VARIANT var1,
    /* [out][retval] */ IXMLElement2 **ppElem);


void __RPC_STUB IXMLDocument2_createElement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_get_async_Proxy( 
    IXMLDocument2 * This,
    /* [out][retval] */ VARIANT_BOOL *pf);


void __RPC_STUB IXMLDocument2_get_async_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLDocument2_put_async_Proxy( 
    IXMLDocument2 * This,
    /* [in] */ VARIANT_BOOL f);


void __RPC_STUB IXMLDocument2_put_async_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLDocument2_INTERFACE_DEFINED__ */


#ifndef __IXMLElement_INTERFACE_DEFINED__
#define __IXMLElement_INTERFACE_DEFINED__

/* interface IXMLElement */
/* [helpstring][hidden][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLElement;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F7F31AC-E15F-11d0-9C25-00C04FC99C8E")
    IXMLElement : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_tagName( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_tagName( 
            /* [in] */ BSTR p) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_parent( 
            /* [out][retval] */ IXMLElement **ppParent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setAttribute( 
            /* [in] */ BSTR strPropertyName,
            /* [in] */ VARIANT PropertyValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getAttribute( 
            /* [in] */ BSTR strPropertyName,
            /* [out][retval] */ VARIANT *PropertyValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE removeAttribute( 
            /* [in] */ BSTR strPropertyName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_children( 
            /* [out][retval] */ IXMLElementCollection **pp) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_type( 
            /* [out][retval] */ long *plType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_text( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_text( 
            /* [in] */ BSTR p) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE addChild( 
            /* [in] */ IXMLElement *pChildElem,
            long lIndex,
            long lReserved) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE removeChild( 
            /* [in] */ IXMLElement *pChildElem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLElementVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLElement * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLElement * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLElement * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLElement * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLElement * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLElement * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLElement * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_tagName )( 
            IXMLElement * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_tagName )( 
            IXMLElement * This,
            /* [in] */ BSTR p);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parent )( 
            IXMLElement * This,
            /* [out][retval] */ IXMLElement **ppParent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setAttribute )( 
            IXMLElement * This,
            /* [in] */ BSTR strPropertyName,
            /* [in] */ VARIANT PropertyValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getAttribute )( 
            IXMLElement * This,
            /* [in] */ BSTR strPropertyName,
            /* [out][retval] */ VARIANT *PropertyValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeAttribute )( 
            IXMLElement * This,
            /* [in] */ BSTR strPropertyName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_children )( 
            IXMLElement * This,
            /* [out][retval] */ IXMLElementCollection **pp);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_type )( 
            IXMLElement * This,
            /* [out][retval] */ long *plType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLElement * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLElement * This,
            /* [in] */ BSTR p);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *addChild )( 
            IXMLElement * This,
            /* [in] */ IXMLElement *pChildElem,
            long lIndex,
            long lReserved);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLElement * This,
            /* [in] */ IXMLElement *pChildElem);
        
        END_INTERFACE
    } IXMLElementVtbl;

    interface IXMLElement
    {
        CONST_VTBL struct IXMLElementVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLElement_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLElement_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLElement_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLElement_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLElement_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLElement_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLElement_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLElement_get_tagName(This,p)	\
    (This)->lpVtbl -> get_tagName(This,p)

#define IXMLElement_put_tagName(This,p)	\
    (This)->lpVtbl -> put_tagName(This,p)

#define IXMLElement_get_parent(This,ppParent)	\
    (This)->lpVtbl -> get_parent(This,ppParent)

#define IXMLElement_setAttribute(This,strPropertyName,PropertyValue)	\
    (This)->lpVtbl -> setAttribute(This,strPropertyName,PropertyValue)

#define IXMLElement_getAttribute(This,strPropertyName,PropertyValue)	\
    (This)->lpVtbl -> getAttribute(This,strPropertyName,PropertyValue)

#define IXMLElement_removeAttribute(This,strPropertyName)	\
    (This)->lpVtbl -> removeAttribute(This,strPropertyName)

#define IXMLElement_get_children(This,pp)	\
    (This)->lpVtbl -> get_children(This,pp)

#define IXMLElement_get_type(This,plType)	\
    (This)->lpVtbl -> get_type(This,plType)

#define IXMLElement_get_text(This,p)	\
    (This)->lpVtbl -> get_text(This,p)

#define IXMLElement_put_text(This,p)	\
    (This)->lpVtbl -> put_text(This,p)

#define IXMLElement_addChild(This,pChildElem,lIndex,lReserved)	\
    (This)->lpVtbl -> addChild(This,pChildElem,lIndex,lReserved)

#define IXMLElement_removeChild(This,pChildElem)	\
    (This)->lpVtbl -> removeChild(This,pChildElem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement_get_tagName_Proxy( 
    IXMLElement * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLElement_get_tagName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLElement_put_tagName_Proxy( 
    IXMLElement * This,
    /* [in] */ BSTR p);


void __RPC_STUB IXMLElement_put_tagName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement_get_parent_Proxy( 
    IXMLElement * This,
    /* [out][retval] */ IXMLElement **ppParent);


void __RPC_STUB IXMLElement_get_parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElement_setAttribute_Proxy( 
    IXMLElement * This,
    /* [in] */ BSTR strPropertyName,
    /* [in] */ VARIANT PropertyValue);


void __RPC_STUB IXMLElement_setAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElement_getAttribute_Proxy( 
    IXMLElement * This,
    /* [in] */ BSTR strPropertyName,
    /* [out][retval] */ VARIANT *PropertyValue);


void __RPC_STUB IXMLElement_getAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElement_removeAttribute_Proxy( 
    IXMLElement * This,
    /* [in] */ BSTR strPropertyName);


void __RPC_STUB IXMLElement_removeAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement_get_children_Proxy( 
    IXMLElement * This,
    /* [out][retval] */ IXMLElementCollection **pp);


void __RPC_STUB IXMLElement_get_children_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement_get_type_Proxy( 
    IXMLElement * This,
    /* [out][retval] */ long *plType);


void __RPC_STUB IXMLElement_get_type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement_get_text_Proxy( 
    IXMLElement * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLElement_get_text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLElement_put_text_Proxy( 
    IXMLElement * This,
    /* [in] */ BSTR p);


void __RPC_STUB IXMLElement_put_text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElement_addChild_Proxy( 
    IXMLElement * This,
    /* [in] */ IXMLElement *pChildElem,
    long lIndex,
    long lReserved);


void __RPC_STUB IXMLElement_addChild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElement_removeChild_Proxy( 
    IXMLElement * This,
    /* [in] */ IXMLElement *pChildElem);


void __RPC_STUB IXMLElement_removeChild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLElement_INTERFACE_DEFINED__ */


#ifndef __IXMLElement2_INTERFACE_DEFINED__
#define __IXMLElement2_INTERFACE_DEFINED__

/* interface IXMLElement2 */
/* [helpstring][hidden][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLElement2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2B8DE2FF-8D2D-11d1-B2FC-00C04FD915A9")
    IXMLElement2 : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_tagName( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_tagName( 
            /* [in] */ BSTR p) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_parent( 
            /* [out][retval] */ IXMLElement2 **ppParent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setAttribute( 
            /* [in] */ BSTR strPropertyName,
            /* [in] */ VARIANT PropertyValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getAttribute( 
            /* [in] */ BSTR strPropertyName,
            /* [out][retval] */ VARIANT *PropertyValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE removeAttribute( 
            /* [in] */ BSTR strPropertyName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_children( 
            /* [out][retval] */ IXMLElementCollection **pp) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_type( 
            /* [out][retval] */ long *plType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_text( 
            /* [out][retval] */ BSTR *p) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_text( 
            /* [in] */ BSTR p) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE addChild( 
            /* [in] */ IXMLElement2 *pChildElem,
            long lIndex,
            long lReserved) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE removeChild( 
            /* [in] */ IXMLElement2 *pChildElem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_attributes( 
            /* [out][retval] */ IXMLElementCollection **pp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLElement2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLElement2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLElement2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLElement2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLElement2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLElement2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLElement2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLElement2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_tagName )( 
            IXMLElement2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_tagName )( 
            IXMLElement2 * This,
            /* [in] */ BSTR p);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_parent )( 
            IXMLElement2 * This,
            /* [out][retval] */ IXMLElement2 **ppParent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setAttribute )( 
            IXMLElement2 * This,
            /* [in] */ BSTR strPropertyName,
            /* [in] */ VARIANT PropertyValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getAttribute )( 
            IXMLElement2 * This,
            /* [in] */ BSTR strPropertyName,
            /* [out][retval] */ VARIANT *PropertyValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeAttribute )( 
            IXMLElement2 * This,
            /* [in] */ BSTR strPropertyName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_children )( 
            IXMLElement2 * This,
            /* [out][retval] */ IXMLElementCollection **pp);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_type )( 
            IXMLElement2 * This,
            /* [out][retval] */ long *plType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLElement2 * This,
            /* [out][retval] */ BSTR *p);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLElement2 * This,
            /* [in] */ BSTR p);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *addChild )( 
            IXMLElement2 * This,
            /* [in] */ IXMLElement2 *pChildElem,
            long lIndex,
            long lReserved);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLElement2 * This,
            /* [in] */ IXMLElement2 *pChildElem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLElement2 * This,
            /* [out][retval] */ IXMLElementCollection **pp);
        
        END_INTERFACE
    } IXMLElement2Vtbl;

    interface IXMLElement2
    {
        CONST_VTBL struct IXMLElement2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLElement2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLElement2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLElement2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLElement2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLElement2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLElement2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLElement2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLElement2_get_tagName(This,p)	\
    (This)->lpVtbl -> get_tagName(This,p)

#define IXMLElement2_put_tagName(This,p)	\
    (This)->lpVtbl -> put_tagName(This,p)

#define IXMLElement2_get_parent(This,ppParent)	\
    (This)->lpVtbl -> get_parent(This,ppParent)

#define IXMLElement2_setAttribute(This,strPropertyName,PropertyValue)	\
    (This)->lpVtbl -> setAttribute(This,strPropertyName,PropertyValue)

#define IXMLElement2_getAttribute(This,strPropertyName,PropertyValue)	\
    (This)->lpVtbl -> getAttribute(This,strPropertyName,PropertyValue)

#define IXMLElement2_removeAttribute(This,strPropertyName)	\
    (This)->lpVtbl -> removeAttribute(This,strPropertyName)

#define IXMLElement2_get_children(This,pp)	\
    (This)->lpVtbl -> get_children(This,pp)

#define IXMLElement2_get_type(This,plType)	\
    (This)->lpVtbl -> get_type(This,plType)

#define IXMLElement2_get_text(This,p)	\
    (This)->lpVtbl -> get_text(This,p)

#define IXMLElement2_put_text(This,p)	\
    (This)->lpVtbl -> put_text(This,p)

#define IXMLElement2_addChild(This,pChildElem,lIndex,lReserved)	\
    (This)->lpVtbl -> addChild(This,pChildElem,lIndex,lReserved)

#define IXMLElement2_removeChild(This,pChildElem)	\
    (This)->lpVtbl -> removeChild(This,pChildElem)

#define IXMLElement2_get_attributes(This,pp)	\
    (This)->lpVtbl -> get_attributes(This,pp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement2_get_tagName_Proxy( 
    IXMLElement2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLElement2_get_tagName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLElement2_put_tagName_Proxy( 
    IXMLElement2 * This,
    /* [in] */ BSTR p);


void __RPC_STUB IXMLElement2_put_tagName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement2_get_parent_Proxy( 
    IXMLElement2 * This,
    /* [out][retval] */ IXMLElement2 **ppParent);


void __RPC_STUB IXMLElement2_get_parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElement2_setAttribute_Proxy( 
    IXMLElement2 * This,
    /* [in] */ BSTR strPropertyName,
    /* [in] */ VARIANT PropertyValue);


void __RPC_STUB IXMLElement2_setAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElement2_getAttribute_Proxy( 
    IXMLElement2 * This,
    /* [in] */ BSTR strPropertyName,
    /* [out][retval] */ VARIANT *PropertyValue);


void __RPC_STUB IXMLElement2_getAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElement2_removeAttribute_Proxy( 
    IXMLElement2 * This,
    /* [in] */ BSTR strPropertyName);


void __RPC_STUB IXMLElement2_removeAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement2_get_children_Proxy( 
    IXMLElement2 * This,
    /* [out][retval] */ IXMLElementCollection **pp);


void __RPC_STUB IXMLElement2_get_children_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement2_get_type_Proxy( 
    IXMLElement2 * This,
    /* [out][retval] */ long *plType);


void __RPC_STUB IXMLElement2_get_type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement2_get_text_Proxy( 
    IXMLElement2 * This,
    /* [out][retval] */ BSTR *p);


void __RPC_STUB IXMLElement2_get_text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXMLElement2_put_text_Proxy( 
    IXMLElement2 * This,
    /* [in] */ BSTR p);


void __RPC_STUB IXMLElement2_put_text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElement2_addChild_Proxy( 
    IXMLElement2 * This,
    /* [in] */ IXMLElement2 *pChildElem,
    long lIndex,
    long lReserved);


void __RPC_STUB IXMLElement2_addChild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IXMLElement2_removeChild_Proxy( 
    IXMLElement2 * This,
    /* [in] */ IXMLElement2 *pChildElem);


void __RPC_STUB IXMLElement2_removeChild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLElement2_get_attributes_Proxy( 
    IXMLElement2 * This,
    /* [out][retval] */ IXMLElementCollection **pp);


void __RPC_STUB IXMLElement2_get_attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLElement2_INTERFACE_DEFINED__ */


#ifndef __IXMLAttribute_INTERFACE_DEFINED__
#define __IXMLAttribute_INTERFACE_DEFINED__

/* interface IXMLAttribute */
/* [helpstring][hidden][oleautomation][dual][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLAttribute;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D4D4A0FC-3B73-11d1-B2B4-00C04FB92596")
    IXMLAttribute : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_name( 
            /* [out][retval] */ BSTR *n) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_value( 
            /* [out][retval] */ BSTR *v) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLAttributeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLAttribute * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLAttribute * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLAttribute * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLAttribute * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLAttribute * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLAttribute * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLAttribute * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_name )( 
            IXMLAttribute * This,
            /* [out][retval] */ BSTR *n);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_value )( 
            IXMLAttribute * This,
            /* [out][retval] */ BSTR *v);
        
        END_INTERFACE
    } IXMLAttributeVtbl;

    interface IXMLAttribute
    {
        CONST_VTBL struct IXMLAttributeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLAttribute_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLAttribute_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLAttribute_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLAttribute_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXMLAttribute_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXMLAttribute_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXMLAttribute_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXMLAttribute_get_name(This,n)	\
    (This)->lpVtbl -> get_name(This,n)

#define IXMLAttribute_get_value(This,v)	\
    (This)->lpVtbl -> get_value(This,v)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLAttribute_get_name_Proxy( 
    IXMLAttribute * This,
    /* [out][retval] */ BSTR *n);


void __RPC_STUB IXMLAttribute_get_name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXMLAttribute_get_value_Proxy( 
    IXMLAttribute * This,
    /* [out][retval] */ BSTR *v);


void __RPC_STUB IXMLAttribute_get_value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLAttribute_INTERFACE_DEFINED__ */


#ifndef __IXMLError_INTERFACE_DEFINED__
#define __IXMLError_INTERFACE_DEFINED__

/* interface IXMLError */
/* [helpstring][hidden][uuid][object][local] */ 


EXTERN_C const IID IID_IXMLError;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("948C5AD3-C58D-11d0-9C0B-00C04FC99C8E")
    IXMLError : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetErrorInfo( 
            XML_ERROR *pErrorReturn) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXMLErrorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLError * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLError * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLError * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetErrorInfo )( 
            IXMLError * This,
            XML_ERROR *pErrorReturn);
        
        END_INTERFACE
    } IXMLErrorVtbl;

    interface IXMLError
    {
        CONST_VTBL struct IXMLErrorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXMLError_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXMLError_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXMLError_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXMLError_GetErrorInfo(This,pErrorReturn)	\
    (This)->lpVtbl -> GetErrorInfo(This,pErrorReturn)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IXMLError_GetErrorInfo_Proxy( 
    IXMLError * This,
    XML_ERROR *pErrorReturn);


void __RPC_STUB IXMLError_GetErrorInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXMLError_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_XMLDocument;

#ifdef __cplusplus

class DECLSPEC_UUID("CFC399AF-D876-11d0-9C10-00C04FC99C8E")
XMLDocument;
#endif
#endif /* __MSXML_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



