#include <iostream>
#include <vector>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>

// Define a class for the mobile tower
class Tower {
public:
    int id;
    Tower* parent;
    std::set<Tower*> children;
    
    Tower(int id) : id(id), parent(nullptr) {}

    void addChild(Tower* child) {
        child->parent = this;
        children.insert(child);
    }
};

// Function to find the Lowest Common Ancestor (LCA) of two towers
Tower* findLCA(Tower* root, Tower* a, Tower* b) {
    if (!root || root == a || root == b) return root;
    
    int count = 0;
    Tower* temp = nullptr;
    
    for (auto child : root->children) {
        Tower* res = findLCA(child, a, b);
        if (res) {
            count++;
            temp = res;
        }
    }
    
    if (count == 2) return root;
    return temp;
}

// Define a class for the mobile phone
class MobilePhone {
public:
    int id;
    Tower* connectedTower;

    MobilePhone(int id, Tower* connectedTower) : id(id), connectedTower(connectedTower) {}

    void moveToTower(Tower* newTower) {
        connectedTower = newTower;
    }
};

// Define the region with head tower and its mobile phones
class Region {
public:
    Tower* headTower;
    std::vector<MobilePhone*> mobilePhones;

    Region(Tower* headTower) : headTower(headTower) {}

    void addMobilePhone(MobilePhone* phone) {
        mobilePhones.push_back(phone);
    }
};

// Synchronization tools
std::mutex mtx;
std::condition_variable cv;

// Function to simulate a phone call between two mobile phones
void makeCall(MobilePhone* caller, MobilePhone* receiver) {
    std::unique_lock<std::mutex> lock(mtx);
    
    // Find the LCA of the towers to route the call
    Tower* lcaTower = findLCA(caller->connectedTower->parent, caller->connectedTower, receiver->connectedTower);
    
    std::cout << "Call routed through tower ID: " << lcaTower->id << " from phone ID: " << caller->id << " to phone ID: " << receiver->id << std::endl;
    
    lock.unlock();
    cv.notify_all();
}

int main() {
    // Create towers for a region
    Tower* headTower = new Tower(1);
    Tower* towerA = new Tower(2);
    Tower* towerB = new Tower(3);
    Tower* towerC = new Tower(4);
    Tower* towerD = new Tower(5);

    // Establish hierarchy (headTower -> towerA, towerB -> towerC, towerD)
    headTower->addChild(towerA);
    headTower->addChild(towerB);
    towerB->addChild(towerC);
    towerB->addChild(towerD);

    // Create a region
    Region* region = new Region(headTower);

    // Create mobile phones connected to different towers
    MobilePhone* phone1 = new MobilePhone(101, towerA);
    MobilePhone* phone2 = new MobilePhone(102, towerC);
    MobilePhone* phone3 = new MobilePhone(103, towerD);
    
    // Add phones to region
    region->addMobilePhone(phone1);
    region->addMobilePhone(phone2);
    region->addMobilePhone(phone3);

    // Simulate concurrent calls using multithreading
    std::thread t1(makeCall, phone1, phone2);
    std::thread t2(makeCall, phone2, phone3);
    
    t1.join();
    t2.join();

    // Cleanup
    delete headTower;
    delete towerA;
    delete towerB;
    delete towerC;
    delete towerD;
    delete region;
    delete phone1;
    delete phone2;
    delete phone3;

    return 0;
}
