#include "zp_meta_client_conn.h"

#include <vector>
#include <algorithm>
#include <glog/logging.h>

#include "zp_meta_worker_thread.h"
#include "zp_meta_server.h"

extern ZPMetaServer* zp_meta_server;

////// ZPDataClientConn //////
ZPMetaClientConn::ZPMetaClientConn(int fd, std::string ip_port, pink::Thread* thread) :
  PbConn(fd, ip_port) {
  self_thread_ = dynamic_cast<ZPMetaWorkerThread*>(thread);
}

ZPMetaClientConn::~ZPMetaClientConn() {
}

// Msg is  [ length (int32) | pb_msg (length bytes) ]
int ZPMetaClientConn::DealMessage() {
  request_.ParseFromArray(rbuf_ + 4, header_len_);
  // TODO test only
  switch (request_.type()) {
		case ZPMeta::MetaCmd_Type::MetaCmd_Type_JOIN: {
      DLOG(INFO) << "Receive join cmd";
      break;
    }
		case ZPMeta::MetaCmd_Type::MetaCmd_Type_PING: {
      DLOG(INFO) << "Receive ping cmd";
      break;
    }
  }

  Cmd* cmd = self_thread_->GetCmd(static_cast<int>(request_.type()));
  if (cmd == NULL) {
    LOG(ERROR) << "unsupported type: " << (int)request_.type();
    return -1;
  }

  cmd->Do(&request_, &response_);
  set_is_reply(true);

  res_ = &response_;

  return 0;
}

