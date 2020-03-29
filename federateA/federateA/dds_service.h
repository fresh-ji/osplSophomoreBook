
#ifndef DDS_SERVICE_H
#define DDS_SERVICE_H

#include <vector>
#include <functional>
#include <map>
#include <thread>
#include <atomic>

#include "ccpp_WaitSetData.h"
#include "ccpp_dds_dcps.h"

using namespace std;
using namespace DDS;
using namespace Messenger;

typedef struct _MsgData {
	long subjectId;
	double time;
	string from;
	string topicName;
	string text;
	double data;
} MsgData;

class CSDDSService  {

public:
	static CSDDSService* Instance();

public:
	bool Init(const string& partition_name);

	bool CreateParticipant(const string& participant_name);
	void DeleteParticipant();

	void RegisterType();
	bool CreateTopic(const string& topic_name);

	bool CreatePublisher();
	bool CreateSubscriber();

	bool CreateWriter(const string& topic_name);
	bool CreateReader(const string& topic_name);

	bool write(const string &topic_name, const MsgData& msg_data);
	vector<MsgData> read(const string &topic_name);
	vector<MsgData> take(const string &topic_name);

	void ReadWithWaitSet();

	void StartReceiveData();
	void StopReceiveData();

	void SetCallBack(function<bool(MsgData)>);

	void Clear();

	DataReader_ptr getReader(const string& topic_name);
	DataWriter_ptr getWriter(const string& topic_name);
	Publisher_ptr getPublisher();
	Subscriber_ptr getSubscriber();
	Topic_ptr getTopic(const string &topic_name);
	DomainParticipant_ptr getParticipant();

	~CSDDSService();
	CSDDSService(const CSDDSService&) = delete;
	CSDDSService& operator=(const CSDDSService&) = delete;

private:
	CSDDSService();
	DomainParticipantFactory_var dpf_;
	DomainParticipant_var participant_;
	DomainId_t domain_id_;
	DDS::String_var partition_name_;
	DDS::String_var type_name_;

	Publisher_var publisher_;
	Subscriber_var subscriber_;

	WaitSet_var newMsgWS;
	Duration_t wait_timeout;

	function<bool(MsgData)> cb_;

	thread read_thread_;
	atomic<bool> read_flag_ = true;

	map<string, Topic_var> topics_;
	map<string, DataReader_var> readers_;
	map<string, DataWriter_var> writers_;
	map<string, ReadCondition_var> conditions_;
};

#endif
