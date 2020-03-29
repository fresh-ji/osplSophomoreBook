//******************************************************************
// 
//  Generated by IDL to C++ Translator
//  
//  File name: WaitSetData.h
//  Source: WaitSetData.idl
//  Generated: Sun Mar 29 12:04:23 2020
//  OpenSplice 6.9.181018OSS
//  
//******************************************************************
#ifndef _WAITSETDATA_H_
#define _WAITSETDATA_H_

#include "sacpp_mapping.h"
#include "examples_export.h"


namespace Messenger
{
   struct Message;

   struct OS_EXAMPLE_API Message
   {
         DDS::Long subjectId;
         DDS::Double time;
         DDS::String_mgr from;
         DDS::String_mgr topicName;
         DDS::String_mgr text;
         DDS::Double data;
   };

   typedef DDS_DCPSStruct_var < Message> Message_var;
   typedef DDS_DCPSStruct_out < Message> Message_out;
}




#endif
