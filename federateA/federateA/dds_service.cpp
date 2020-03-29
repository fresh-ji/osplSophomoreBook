
#include "dds_service.h"
#include "check_status.h"

CSDDSService::CSDDSService() {}

CSDDSService::~CSDDSService() {
	Clear();
}

CSDDSService* CSDDSService::Instance() {
	static CSDDSService service;
	return &service;
}

bool CSDDSService::Init(const string& partition_name) {

	if (!CreateParticipant(partition_name)) {
		return false;
	}

	if (!CreatePublisher()) {
		return false;
	}

	if (!CreateSubscriber()) {
		return false;
	}

	RegisterType();

	newMsgWS = new WaitSet();
	cout << "dds init success" << endl;
	return true;
}

bool CSDDSService::CreateParticipant(
	const string&participant_name) {

	domain_id_ = DOMAIN_ID_DEFAULT;
	dpf_ = DomainParticipantFactory::get_instance();
	if (!CheckHandle(dpf_.in(), "DDS::DomainParticipantFactory::get_instance")) {
		return false;
	}

	participant_ = dpf_->create_participant(domain_id_, PARTICIPANT_QOS_DEFAULT,
		NULL, STATUS_MASK_NONE);
	if (!CheckHandle(participant_.in(),
		"DDS::DomainParticipantFactory::create_participant")) {
		return false;
	}
	partition_name_ = participant_name.c_str();
	return true;
}

void CSDDSService::DeleteParticipant() {

	read_flag_ = false;
	if (read_thread_.joinable()){
		read_thread_.join();
	}

	auto status = dpf_->delete_participant(participant_.in());
	if (!CheckStatus(status, "DDS::DomainParticipant::delete_participant ")) {
		return;
	}
}

bool CSDDSService::CreateTopic(const string& topic_name) {

	auto it = topics_.find(topic_name);
	if (it != topics_.end()){
		return true;
	}

	TopicQos reliable_topic_qos;
	auto status = participant_->get_default_topic_qos(reliable_topic_qos);
	if (!CheckStatus(status, "DDS::DomainParticipant::get_default_topic_qos")) {
		return false;
	}

	reliable_topic_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
	// reliable_topic_qos.durability.kind = TRANSIENT_DURABILITY_QOS;
	reliable_topic_qos.history.kind = KEEP_ALL_HISTORY_QOS;

	status = participant_->set_default_topic_qos(reliable_topic_qos);
	if (!CheckStatus(status, "DDS::DomainParticipant::set_default_topic_qos")) {
		return false;
	}

	Topic_var topic = participant_->create_topic(topic_name.c_str(),
		type_name_, reliable_topic_qos, NULL, STATUS_MASK_NONE);

	if (!CheckHandle(topic.in(), "DDS::DomainParticipant::create_topic ()")) {
		return false;
	}

	topics_[topic_name] = topic;
	return true;
}

void CSDDSService::RegisterType() {
	MessageTypeSupport_var mt = new MessageTypeSupport();
	type_name_ = mt->get_type_name();
	auto status = mt->register_type(participant_.in(), type_name_);
	CheckStatus(status, "register_type");
}

bool CSDDSService::CreatePublisher() {
	PublisherQos pub_qos;
	auto status = participant_->get_default_publisher_qos(pub_qos);
	if (!CheckStatus(status, "DDS::DomainParticipant::get_default_publisher_qos")) {
		return false;
	}

	pub_qos.partition.name.length(1);
	pub_qos.partition.name[0] = partition_name_;

	publisher_ = participant_->create_publisher(pub_qos, NULL, STATUS_MASK_NONE);
	if (!CheckHandle(publisher_.in(), "DDS::DomainParticipant::create_publisher")) {
		return false;
	}
	return true;
}

bool CSDDSService::CreateSubscriber() {
	SubscriberQos sub_qos;
	int status = participant_->get_default_subscriber_qos(sub_qos);
	if (!CheckStatus(status,
		"DDS::DomainParticipant::get_default_subscriber_qos")) {
		return false;
	}

	sub_qos.partition.name.length(1);
	sub_qos.partition.name[0] = partition_name_;

	subscriber_ = participant_->create_subscriber(sub_qos, NULL, STATUS_MASK_NONE);
	if (!CheckHandle(subscriber_.in(),
		"DDS::DomainParticipant::create_subscriber")) {
		return false;
	}
	return true;
}

bool CSDDSService::CreateWriter(const string& topic_name) {

	auto it = writers_.find(topic_name);
	if (it != writers_.end()){
		return true;
	}

	Topic_var topic;
	auto i = topics_.find(topic_name);
	if (i == topics_.end()){
		return false;
	}
	else {
		topic = i->second;
	}

	DDS::DataWriterQos dw_qos;
	auto status = publisher_->get_default_datawriter_qos(dw_qos);
	if (!CheckStatus(status, "DDS::DomainParticipant::get_default_publisher_qos")) {
		return false;
	}

	TopicQos reliable_topic_qos;
	status = participant_->get_default_topic_qos(reliable_topic_qos);

	status = publisher_->copy_from_topic_qos(dw_qos, reliable_topic_qos);
	CheckStatus(status, "DDS::Publisher::copy_from_topic_qos");
	dw_qos.writer_data_lifecycle.autodispose_unregistered_instances = true;

	DataWriter_var writer = publisher_->create_datawriter(topic.in(), dw_qos,
		NULL, STATUS_MASK_NONE);
	if (!CheckHandle(writer, "DDS::Publisher::create_datawriter")) {
		return false;
	}

	writers_[topic_name] = writer;
	return true;
}

bool CSDDSService::CreateReader(const string& topic_name) {

	auto it = readers_.find(topic_name);
	if (it != readers_.end()){
		return true;
	}

	Topic_var topic;
	auto i = topics_.find(topic_name);
	if (i == topics_.end()){
		return false;
	}
	else {
		topic = i->second;
	}

	DataReader_var reader = subscriber_->create_datareader(topic.in(),
		DATAREADER_QOS_USE_TOPIC_QOS, NULL, STATUS_MASK_NONE);
	if (!CheckHandle(reader, "DDS::Subscriber::create_datareader ()")) {
		return false;
	}

	DDS::Property pp;
	pp.name = "ignoreLoansOnDeletion";
	pp.value = "true";
	reader->set_property(pp);

	ReadCondition_var newMsg = reader->create_readcondition(ANY_SAMPLE_STATE,
		ANY_VIEW_STATE, ANY_INSTANCE_STATE);
	if (!CheckHandle(newMsg.in(), "DDS::DataReader::create_readcondition")) {
		return false;
	}

	auto status = newMsgWS->attach_condition(newMsg.in());
	if (!CheckStatus(status, "DDS::WaitSetData::attach_condition (newMsg)")){
		return false;
	}

	conditions_[topic_name] = newMsg;
	readers_[topic_name] = reader;
	return true;
}

bool CSDDSService::write(const string &topic_name, const MsgData& msg_data) {

	auto it = writers_.find(topic_name);
	if (it == writers_.end()){
		return false;
	}

	DataWriter_var writer = it->second;

	Message msgInstance;
	msgInstance.subjectId = msg_data.subjectId;
	msgInstance.time = msg_data.time;
	msgInstance.from = msg_data.from.c_str();
	msgInstance.topicName = msg_data.topicName.c_str();
	msgInstance.text = msg_data.text.c_str();
	msgInstance.data = msg_data.data;

	MessageDataWriter_var HelloWorldWriter = MessageDataWriter::_narrow(writer.in());

	auto status = HelloWorldWriter->write(msgInstance, DDS::HANDLE_NIL);
	if (!CheckStatus(status, "MsgDataWriter::write")) {
		return false;
	}

	return true;
}

vector<MsgData> CSDDSService::read(const string &topic_name) {

	auto it = readers_.find(topic_name);
	DataReader_var reader = it->second;

	MessageDataReader_var HelloWorldReader = MessageDataReader::_narrow(reader.in());
	CheckHandle(HelloWorldReader.in(), "MsgDataReader::_narrow");

	MessageSeq msgList;
	SampleInfoSeq infoSeq;
	vector<MsgData> dataqueue;

	auto status = HelloWorldReader->read(msgList, infoSeq, LENGTH_UNLIMITED,
		ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
	CheckStatus(status, "msgDataReader::take");
	for (DDS::ULong j = 0; j < msgList.length(); j++) {
		MsgData data;
		data.subjectId = msgList[j].subjectId;
		data.time = msgList[j].time;
		data.from = msgList[j].from;
		data.topicName = msgList[j].topicName;
		data.text = msgList[j].text;
		data.data = msgList[j].data;
		dataqueue.push_back(data);
	}

	return dataqueue;
}

vector<MsgData> CSDDSService::take(const string &topic_name) {

	auto it = readers_.find(topic_name);
	DataReader_var reader = it->second;

	MessageDataReader_var HelloWorldReader = MessageDataReader::_narrow(reader.in());
	CheckHandle(HelloWorldReader.in(), "MsgDataReader::_narrow");

	MessageSeq msgList;
	SampleInfoSeq infoSeq;
	vector<MsgData> dataqueue;

	auto status = HelloWorldReader->take(msgList, infoSeq, LENGTH_UNLIMITED,
		ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
	CheckStatus(status, "msgDataReader::take");
	for (DDS::ULong j = 0; j < msgList.length(); j++) {
		MsgData data;
		data.subjectId = msgList[j].subjectId;
		data.time = msgList[j].time;
		data.from = msgList[j].from;
		data.topicName = msgList[j].topicName;
		data.text = msgList[j].text;
		data.data = msgList[j].data;
		dataqueue.push_back(data);
	}

	return dataqueue;
}

void CSDDSService::ReadWithWaitSet() {
	ConditionSeq guardList;
	guardList.length(conditions_.size());
	MessageSeq msgList;
	SampleInfoSeq infoSeq;

	wait_timeout.sec = 2;
	wait_timeout.nanosec = 0;

	while (read_flag_){
		auto status = newMsgWS->wait(guardList, wait_timeout);
		if (status == DDS::RETCODE_OK) {
			for (DDS::ULong i = 0; i < guardList.length(); i++) {
				for (auto it : conditions_){
					auto topic_name = it.first;
					auto condition = it.second;

					if (guardList[i].in() == condition.in()) {
						auto reader = readers_[topic_name];
						MessageDataReader_var MsgReader = MessageDataReader::_narrow(reader.in());
						CheckHandle(MsgReader.in(), "MsgDataReader::_narrow");
						status = MsgReader->take_w_condition(msgList, infoSeq, LENGTH_UNLIMITED, condition.in());
						CheckStatus(status, "WaitSetData::MsgDataReader::take_w_condition");

						for (DDS::ULong j = 0; j < msgList.length(); j++) {
							if (infoSeq[j].valid_data) {
								MsgData data;
								data.subjectId = msgList[j].subjectId;
								data.time = msgList[j].time;
								data.from = msgList[j].from;
								data.topicName = msgList[j].topicName;
								data.text = msgList[j].text;
								data.data = msgList[j].data;

								if (cb_ != nullptr) {
									cb_(data);
								}
							}
						}

						if (msgList.length() > 0) {
							status = MsgReader->return_loan(msgList, infoSeq);
							CheckStatus(status, "WaitSetData::MsgDataReader::return_loan");
						}
					}
				}
			}
		}
		else if (status != DDS::RETCODE_TIMEOUT) {
			CheckStatus(status, "DDS::WaitSetData::wait");
		}
	}
}

void CSDDSService::StartReceiveData() {
	read_flag_ = true;
	read_thread_ = thread(&CSDDSService::ReadWithWaitSet, this);
	read_thread_.detach();
}

void CSDDSService::StopReceiveData() {
	if (read_flag_){
		read_flag_ = false;
	}
}

void CSDDSService::SetCallBack(function<bool(MsgData)> cb) {
	cb_ = cb;
}

void CSDDSService::Clear() {
	read_flag_ = false;
	try {
		if (read_thread_.joinable()) {
			read_thread_.join();
		}
	}
	catch (const exception& e) {
	}

	ReturnCode_t rv = 0;

	if (participant_) {
		rv = participant_->delete_contained_entities();
	}

	if (dpf_) {
		rv = dpf_->delete_participant(participant_);
	}

	topics_.clear();
	writers_.clear();
	readers_.clear();
	conditions_.clear();

	participant_ = nullptr;
	domain_id_ = 0;
	partition_name_ = "";
	type_name_ = "";

	publisher_ = nullptr;
	subscriber_ = nullptr;
	newMsgWS = nullptr;
	memset(&wait_timeout, 0, sizeof(wait_timeout));
}

DataReader_ptr CSDDSService::getReader(const string& topic_name) {
	auto it = readers_.find(topic_name);
	if (it != readers_.end()){
		return DataReader::_duplicate(it->second.in());
	}
}

DataWriter_ptr CSDDSService::getWriter(const string& topic_name) {
	auto it = writers_.find(topic_name);
	if (it != writers_.end()){
		return DataWriter::_duplicate(it->second.in());
	}
}

Publisher_ptr CSDDSService::getPublisher() {
	return Publisher::_duplicate(publisher_.in());
}

Subscriber_ptr CSDDSService::getSubscriber() {
	return Subscriber::_duplicate(subscriber_.in());
}

Topic_ptr CSDDSService::getTopic(const string &topic_name) {
	auto it = topics_.find(topic_name);
	if (it != topics_.end()){
		return Topic::_duplicate(it->second.in());
	}
}

DomainParticipant_ptr CSDDSService::getParticipant() {
	return DomainParticipant::_duplicate(participant_.in());
}
