#ifndef WAITSETDATASPLTYPES_H
#define WAITSETDATASPLTYPES_H

#include <c_base.h>
#include <c_misc.h>
#include <c_sync.h>
#include <c_collection.h>
#include <c_field.h>
#include <v_copyIn.h>

#include "ccpp_WaitSetData.h"
#include "examples_export.h"

extern c_metaObject __WaitSetData_Messenger__load (c_base base);

extern const char *Messenger_Message_metaDescriptor[];
extern const int Messenger_Message_metaDescriptorArrLength;
extern const int Messenger_Message_metaDescriptorLength;
extern c_metaObject __Messenger_Message__load (c_base base);
struct _Messenger_Message ;
extern OS_EXAMPLE_API v_copyin_result __Messenger_Message__copyIn(c_base base, const struct Messenger::Message *from, struct _Messenger_Message *to);
extern OS_EXAMPLE_API void __Messenger_Message__copyOut(const void *_from, void *_to);
struct _Messenger_Message {
    c_long subjectId;
    c_double time;
    c_string from;
    c_string topicName;
    c_string text;
    c_double data;
};

#undef OS_API
#endif
