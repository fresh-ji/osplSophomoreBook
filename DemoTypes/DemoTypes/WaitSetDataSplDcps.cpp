#include "WaitSetDataSplDcps.h"
#include "ccpp_WaitSetData.h"

#include <v_copyIn.h>
#include <v_topic.h>
#include <os_stdlib.h>
#include <string.h>
#include <os_report.h>

v_copyin_result
__Messenger_Message__copyIn(
    c_base base,
    const struct ::Messenger::Message *from,
    struct _Messenger_Message *to)
{
    v_copyin_result result = V_COPYIN_RESULT_OK;
    (void) base;

    to->subjectId = (c_long)from->subjectId;
    to->time = (c_double)from->time;
#ifdef OSPL_BOUNDS_CHECK
    if(from->from){
        to->from = c_stringNew_s(base, from->from);
        if(to->from == NULL) {
            result = V_COPYIN_RESULT_OUT_OF_MEMORY;
        }
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'Messenger::Message.from' of type 'c_string' is NULL.");
        result = V_COPYIN_RESULT_INVALID;
    }
#else
    to->from = c_stringNew_s(base, from->from);
    if(to->from == NULL) {
        result = V_COPYIN_RESULT_OUT_OF_MEMORY;
    }
#endif
#ifdef OSPL_BOUNDS_CHECK
    if(from->topicName){
        to->topicName = c_stringNew_s(base, from->topicName);
        if(to->topicName == NULL) {
            result = V_COPYIN_RESULT_OUT_OF_MEMORY;
        }
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'Messenger::Message.topicName' of type 'c_string' is NULL.");
        result = V_COPYIN_RESULT_INVALID;
    }
#else
    to->topicName = c_stringNew_s(base, from->topicName);
    if(to->topicName == NULL) {
        result = V_COPYIN_RESULT_OUT_OF_MEMORY;
    }
#endif
#ifdef OSPL_BOUNDS_CHECK
    if(from->text){
        to->text = c_stringNew_s(base, from->text);
        if(to->text == NULL) {
            result = V_COPYIN_RESULT_OUT_OF_MEMORY;
        }
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'Messenger::Message.text' of type 'c_string' is NULL.");
        result = V_COPYIN_RESULT_INVALID;
    }
#else
    to->text = c_stringNew_s(base, from->text);
    if(to->text == NULL) {
        result = V_COPYIN_RESULT_OUT_OF_MEMORY;
    }
#endif
    to->data = (c_double)from->data;
    return result;
}

void
__Messenger_Message__copyOut(
    const void *_from,
    void *_to)
{
    const struct _Messenger_Message *from = (const struct _Messenger_Message *)_from;
    struct ::Messenger::Message *to = (struct ::Messenger::Message *)_to;
    to->subjectId = (::DDS::Long)from->subjectId;
    to->time = (::DDS::Double)from->time;
    to->from = DDS::string_dup(from->from ? from->from : "");
    to->topicName = DDS::string_dup(from->topicName ? from->topicName : "");
    to->text = DDS::string_dup(from->text ? from->text : "");
    to->data = (::DDS::Double)from->data;
}

