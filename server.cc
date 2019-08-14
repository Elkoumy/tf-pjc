/*
 * Copyright 2019 Google Inc.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <memory>
#include <string>
#include <thread>  // NOLINT

#include "include/grpc/grpc_security_constants.h"
#include "include/grpcpp/grpcpp.h"
#include "include/grpcpp/security/server_credentials.h"
#include "include/grpcpp/server_builder.h"
#include "include/grpcpp/server_context.h"
#include "include/grpcpp/support/status.h"
#include "data_util.h"
#include "server_impl.h"
#include "private_join_and_compute.grpc.pb.h"
#include "private_join_and_compute_rpc_impl.h"
#include "protocol_server.h"
#include "absl/memory/memory.h"

int RunServer(std::string port) {
  std::cout << "Server: loading data... " << std::endl;
  auto maybe_server_identifiers = ::private_join_and_compute::ReadServerDatasetFromFile("/tmp/dummy_server_data.csv");
  if (!maybe_server_identifiers.ok()) {
    return 1;
  }

  ::private_join_and_compute::Context context;
  std::unique_ptr<::private_join_and_compute::ProtocolServer> server =
      absl::make_unique<::private_join_and_compute::PrivateIntersectionSumProtocolServerImpl>(
          &context, std::move(maybe_server_identifiers.ValueOrDie()));
  ::private_join_and_compute::PrivateJoinAndComputeRpcImpl service(std::move(server));

  ::grpc::ServerBuilder builder;
  // Consider grpc::SslServerCredentials if not running locally.
  builder.AddListeningPort(port,
                           ::grpc::experimental::LocalServerCredentials(
                               grpc_local_connect_type::LOCAL_TCP));
  builder.RegisterService(&service);
  std::unique_ptr<::grpc::Server> grpc_server(builder.BuildAndStart());

  // Run the server on a background thread.
  std::thread grpc_server_thread(
      [](::grpc::Server* grpc_server_ptr) {
        grpc_server_ptr->Wait();
      },
      grpc_server.get());

  while (!service.protocol_finished()) {
    // Wait for the server to be done, and then shut the server down.
  }

  // Shut down server.
  grpc_server->Shutdown();
  grpc_server_thread.join();
  std::cout << "Server completed protocol and shut down." << std::endl;

  return 0;
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);

  return RunServer("0.0.0.0:10501");
}
