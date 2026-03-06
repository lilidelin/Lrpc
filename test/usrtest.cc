#include"user.pb.h"
#include<iostream>
#include<string>
using namespace std;
using namespace Luser;
int main(){
    RegisterReq req;
    req.set_id(1);
    req.set_username("testuser");
    req.set_password("1234567");

    // Serialize the message to a string
    string serialized_data;
    if(!req.SerializeToString(&serialized_data)) {
        cerr << "Failed to serialize message." << endl;
        return -1;
    }
    cout << "Serialized data: " << serialized_data << endl;

    // Deserialize the message from the string
    RegisterReq deserialized_req;
    if(!deserialized_req.ParseFromString(serialized_data)) {
        cerr<<"Failed to deserialize message."<<endl;
        return -1;
    }
    cout<<"id: "<<deserialized_req.id()<<endl;
    cout<<"username: "<<deserialized_req.username()<<endl;
    cout<<"password: "<<deserialized_req.password()<<endl;
    return 0;
}