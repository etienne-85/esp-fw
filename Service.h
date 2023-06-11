/**
 * SERVICES TREE
 * -> Webserver
 *.       -> server 80
 *              -> OTA service
 *              -> Static server
 *              -> Websockets
 *                  -> /gpio
 *                  -> /dashboard
 *        -> server 443
 * -> FS
 *
 * Instanciate at once
 * services.webserver.port(80).websocket.route("/gpio")
 * services.webserver.port(80).staticserver.route("/public")
 * services.webserver.port(80).otaservice.route("/update")

 services.webserver<80>.websocket<"/gpio">
 *
 * Access to GPIO Service
 * ServiceTree::services.find("webserver").servers.find(80).ws("/gpio")
 * 
 * ServiceTree print leaves: 
 * - http:80/update
 * - http:80/public
 * - ws:80/gpio
 * - ws:80/dashboard
 */

 class Service {
public:
  static std::map<std::string, Service *> *services;
  // static Service *findService(const char *serviceName);
  std::string serviceId;
  std::string serviceTreePath = "";
  Service(const char *serviceId);
  void log(std::string log);
  static void printAll();
  // void print();
  // void buildServiceTreePath();
  virtual void printServiceTree(int depth);
  static void printServiceTree(std::string dummyArg);
};

std::map<std::string, Service *> *Service::instances = new std::map<std::string, Service *>();

Service::Service(const char *serviceId): serviceId(serviceId) {
  (*services)[serviceId]=this;
  // std::cout << "[Service] new service " << this->getServiceName() << " #"
  //           << instances->size() << std::endl;
  // printLog(" #" + std::to_string(instances->size()));
  std::cout << "[Service] " << this->serviceId << " created " << std::endl;
}

// Service *Service::findService(const char *serviceName){
//   Service::instances->find(serviceName);
// }

void Service::log(std::string log) {
  std::cout << "[" << this->serviceId << "]" << log << std::endl;
}
// static
void Service::printAll() {
  std::cout << "*** Running services *** "<< std::endl;
  for (const auto& [id, instance] : *services) {
    std::cout << "- " << id << std::endl;
  }
  std::cout << "************************"<< std::endl;

}

// instance

// static version with no args
void Service::printServiceTree(std::string dummyArg) {
  std::cout << "[Services Tree]" << std::endl;
  for (const auto& [id, instance] : *services) {
    instance->printServiceTree(1);
  }
}

// instance version called for each child service
void Service::printServiceTree(int depth) {
  // compute offset from depth
  std::cout << serviceTreePath << std::endl;
  // std::cout << "[Services]"<< instances->size() << std::endl;
  // for (auto i : *instances) {
  //   i->printServiceTree(depth + 1);
  // }
}