#pragma once

namespace ipms {

struct process {
  static const int kBufferSize = 4024;
  int sock;
  int status;
  int response_code;
  int fd;
  int read_pos;
  int write_pos;
  int total_length;
  char buf[kBufferSize];
};

int setNonblocking(int fd);

process* find_process_by_sock(int sock);

process* accept_sock(int listen_sock);

void read_request(process* process);

void send_response_header(process *process);

void send_response(process *process);

void cleanup(process *process);

void handle_error(process *process, const char* error_string);

void reset_process(process *process);

int open_file(char *filename);

}  // namespace ipms

