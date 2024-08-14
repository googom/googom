//
// Created by root on 8/13/24.
//

#ifndef GOOGOM_SUBSCRIPTION_MANAGER_H
#define GOOGOM_SUBSCRIPTION_MANAGER_H

#include <urcu.h>                   // RCU basics
#include <urcu/rculist.h>           // RCU list support
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <boost/optional.hpp>
#include "../communication/tcp/subscription.h"

struct rcu_subscription {
    struct cds_list_head node;  // RCU list node
    Subscription sub;

    rcu_subscription(const std::string& topicName, const std::string& subscriptionId)
            : sub{topicName, subscriptionId} {
        CDS_INIT_LIST_HEAD(&node);
    }
};

class SubscriptionManager {
public:
    // Delete copy constructor and assignment operator
    SubscriptionManager(const SubscriptionManager&) = delete;
    SubscriptionManager& operator=(const SubscriptionManager&) = delete;

    // Get the singleton instance
    static SubscriptionManager& getInstance() {
        static SubscriptionManager instance;
        return instance;
    }

    // Add a subscription
    void addSubscription(const std::string& topicName, const std::string& subscriptionId) {
        auto new_sub = new rcu_subscription{topicName, subscriptionId};

        // Start an RCU read-side critical section
        rcu_read_lock();

        // Add the new subscription to the list in an RCU-safe manner
        cds_list_add_rcu(&new_sub->node, &head);

        // End the RCU read-side critical section
        rcu_read_unlock();
    }

    // Remove a subscription
    void removeSubscription(const std::string& subscriptionId) {
        rcu_subscription* entry;

        // Start an RCU read-side critical section
        rcu_read_lock();

        cds_list_for_each_entry_rcu(entry, &head, node) {
            if (entry->sub.subscriptionId == subscriptionId) {
                cds_list_del_rcu(&entry->node); // Safe deletion in RCU
                synchronize_rcu(); // Ensure all readers are done
                delete entry; // Safe to delete
                break;
            }
        }

        // End the RCU read-side critical section
        rcu_read_unlock();
    }


    // Find a subscription by ID
    boost::optional<Subscription> findSubscriptionById(const std::string& subscriptionId) const {
        boost::optional<Subscription> result;

        rcu_read_lock();
        cds_list_for_each_rcu(pos, &head) {
            rcu_subscription* entry = cds_list_entry(pos, rcu_subscription, node);
            if (entry->sub.subscriptionId == subscriptionId) {
                result = entry->sub;
                break;
            }
        }
        rcu_read_unlock();

        return result;
    }

    // Find subscriptions by topic
    std::vector<Subscription> findSubscriptionsByTopic(const std::string& topicName) const {
        std::vector<Subscription> result;

        rcu_read_lock();
        cds_list_for_each_rcu(pos, &head) {
            rcu_subscription* entry = cds_list_entry(pos, rcu_subscription, node);
            if (entry->sub.topicName == topicName) {
                result.push_back(entry->sub);
            }
        }
        rcu_read_unlock();

        return result;
    }

    // Get all subscriptions
    std::vector<Subscription> getAllSubscriptions() const {
        std::vector<Subscription> result;

        rcu_read_lock();
        cds_list_for_each_rcu(pos, &head) {
            rcu_subscription* entry = cds_list_entry(pos, rcu_subscription, node);
            result.push_back(entry->sub);
        }
        rcu_read_unlock();

        return result;
    }

private:
    SubscriptionManager() {
        CDS_INIT_LIST_HEAD(&head);
    }

    ~SubscriptionManager() {
        rcu_read_lock();
        cds_list_for_each_rcu(pos, &head) {
            rcu_subscription* entry = cds_list_entry(pos, rcu_subscription, node);
            delete entry;
        }
        rcu_read_unlock();
    }

    struct cds_list_head head;
    mutable struct cds_list_head* pos; // This needs to be mutable to allow modification in const functions
};

#endif // GOOGOM_SUBSCRIPTION_MANAGER_H


