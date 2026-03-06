#include"LrpcHeader.pb.h"
#include<iostream>
#include<string>
using namespace std;
using namespace Lrpc;
int main() {
    RpcHeader header;
    header.set_service_name("TestService");
    header.set_method_name("TestMethod");
    header.set_args_size(123);

    // Serialize the message to a string
    string serialized_data;
    if (!header.SerializeToString(&serialized_data)) {
        cerr << "Failed to serialize message." << endl;
        return -1;
    }

    // Deserialize the message from the string
    RpcHeader deserialized_header;
    if (!deserialized_header.ParseFromString(serialized_data)) {
        cerr << "Failed to deserialize message." << endl;
        return -1;
    }

    // Print the deserialized values
    cout << "Service Name: " << deserialized_header.service_name() << endl;
    cout << "Method Name: " << deserialized_header.method_name() << endl;
    cout << "Args Size: " << deserialized_header.args_size() << endl;

    return 0;
}