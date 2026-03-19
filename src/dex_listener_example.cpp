#include "dex_listener_example.h"

#include "dex_utils.h"
#include "order_entry_common.h"
#include "spdlog/spdlog.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <list>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

void dex_callbacks::config()
{
   std::ifstream             configFile("config.json");
   rapidjson::IStreamWrapper isw{configFile};

   rapidjson::Document config{};
   if (config.ParseStream(isw).HasParseError())
   {
      SPDLOG_ERROR("Error in parsing config.json");
      exit(1);
   }

   std::list<rakurai::dex_connector::configuration::dex_config> dex_config_list;
   rakurai::dex_connector::configuration::logging_config        logging_config;
   rakurai::dex_connector::configuration::session_config        session_config;
   rakurai::dex_connector::configuration::wallet                wallet_config;
   bool                                                         cluster_mode = false;

   if (config.HasMember("app_logging") == true)
   {
      logging_config.severity           = config["app_logging"]["severity"].GetString();
      logging_config.is_console_enabled = config["app_logging"]["console"].GetBool();
      logging_config.is_file_enabled    = config["app_logging"]["file"].GetBool();
      if (config["app_logging"].HasMember("queuesize"))
      {
         logging_config.queue_size = config["app_logging"]["queuesize"].GetInt();
      }

      // Setting example application logging level
      if (logging_config.severity == "trace")
      {
         spdlog::set_level(spdlog::level::trace);
      }
      else if (logging_config.severity == "debug")
      {
         spdlog::set_level(spdlog::level::debug);
      }
      else if (logging_config.severity == "info")
      {
         spdlog::set_level(spdlog::level::info);
      }
      else if (logging_config.severity == "warning")
      {
         spdlog::set_level(spdlog::level::warn);
      }
      else if (logging_config.severity == "error")
      {
         spdlog::set_level(spdlog::level::err);
      }
      else
      {
         spdlog::set_level(spdlog::level::info);
      }
   }
   else
   {
      SPDLOG_INFO("No logging information: Switching to default");
   }

   if (config.HasMember("cluster_mode"))
   {
      cluster_mode = config["cluster_mode"].GetBool();
   }

   if (cluster_mode)
   {
      rakurai::dex_connector::configuration::uri_config loc_1, loc_2;

      if (config.HasMember("md_server_uri_1") == true && config.HasMember("node_oms_uri_1") &&
          config.HasMember("md_server_uri_2") == true && config.HasMember("node_oms_uri_2"))
      {
         loc_1.md_server_uri = config["md_server_uri_1"].GetString();
         loc_1.node_oms_uri  = config["node_oms_uri_1"].GetString();
         loc_2.md_server_uri = config["md_server_uri_2"].GetString();
         loc_2.node_oms_uri  = config["node_oms_uri_2"].GetString();
         session_config.uri.push_back(loc_1);
         session_config.uri.push_back(loc_2);
      }
      else
      {
         SPDLOG_ERROR("Uri config is incomplete");
      }
   }
   else
   {
      rakurai::dex_connector::configuration::uri_config loc_1;
      if (config.HasMember("md_server_uri_1") == true && config.HasMember("node_oms_uri_1"))
      {
         loc_1.md_server_uri = config["md_server_uri_1"].GetString();
         loc_1.node_oms_uri  = config["node_oms_uri_1"].GetString();
         session_config.uri.push_back(loc_1);
      }
      else
      {
         SPDLOG_ERROR("Uri config is incomplete");
      }
   }
   // Add a default URI value

   if (config.HasMember("wallet_pub") == true)
   {
      auto wallet_str              = config["wallet_pub"].GetString();
      wallet_config.wallet_pub_key = rakurai::dex_connector::solanaAddressToBytes(wallet_str);
   }
   else
   {
   }

   if (config.HasMember("wallet_pvt") && config["wallet_pvt"].IsArray())
   {
      const auto &arr = config["wallet_pvt"];
      if (arr.Size() == 64)
      {
         for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
         {
            wallet_config.wallet_pvt_key[i] = static_cast<uint8_t>(arr[i].GetUint());
         }
      }
      else
      {
         SPDLOG_ERROR("'pvtkey' must be an array of 64 elements");
      }
   }
   else
   {
      SPDLOG_ERROR("No Wallet provided or invalid key");
   }

   if (config.HasMember("core") == true)
   {
      session_config.core = config["core"].GetUint();
      // Add a default URI value
   }

   auto dex_configuration = config["dex"].GetArray();

   for (rapidjson::Value::ConstValueIterator dex_configuration_itr = dex_configuration.Begin();
        dex_configuration_itr != dex_configuration.End(); ++dex_configuration_itr)
   {
      rakurai::dex_connector::configuration::dex_config dex_config;

      auto        config = dex_configuration_itr->GetObject();
      std::string dex    = config["id"].GetString();

      if (dex.compare("orca_clmm") == 0)
      {
         dex_config.dex_name = rakurai::dex_connector::data_types::venue::orca_clmm;
         dex_config.pool_typ = rakurai::dex_connector::data_types::pool_type::clmm;
      }
      else if (dex.compare("orca_amm") == 0)
      {
         dex_config.dex_name = rakurai::dex_connector::data_types::venue::orca_amm;
         dex_config.pool_typ = rakurai::dex_connector::data_types::pool_type::amm;
      }
      else if (dex.compare("raydium_amm") == 0)
      {
         dex_config.dex_name = rakurai::dex_connector::data_types::venue::raydium_amm;
         dex_config.pool_typ = rakurai::dex_connector::data_types::pool_type::amm;
      }
      else if (dex.compare("raydium_cpmm") == 0)
      {
         dex_config.dex_name = rakurai::dex_connector::data_types::venue::raydium_cpmm;
         dex_config.pool_typ = rakurai::dex_connector::data_types::pool_type::cpmm;
      }
      else if (dex.compare("raydium_clmm") == 0)
      {
         dex_config.dex_name = rakurai::dex_connector::data_types::venue::raydium_clmm;
         dex_config.pool_typ = rakurai::dex_connector::data_types::pool_type::clmm;
      }
      else
      {
         SPDLOG_ERROR("Unknown dex name specified. Skipping: {}", dex);
         continue;
      }

      if (config["pool_ids"].GetArray().Empty() == false)
      {
         for (rapidjson::Value::ConstValueIterator pool_itr = config["pool_ids"].GetArray().Begin();
              pool_itr != config["pool_ids"].GetArray().End(); ++pool_itr)
         {
            auto pool_id_bytes = rakurai::dex_connector::solanaAddressToBytes(pool_itr->GetString());

            SPDLOG_DEBUG("Adding pool ID to config: {}", [&]() {
               std::ostringstream oss;
               for (unsigned char each_byte : pool_id_bytes)
               {
                  oss << std::setw(2) << std::setfill('0') << static_cast<int>(each_byte) << " ";
               }
               return oss.str();
            }());

            dex_config.interested_pools.emplace_back(pool_id_bytes);
         }
      }
      dex_config_list.push_back(dex_config);
   }

   // Print the logs

   _dex_handler = std::make_shared<rakurai::dex_connector::dex_handler>(
      dex_config_list, wallet_config, logging_config, session_config.uri, session_config.core, cluster_mode, this);
   _trade_sess_interface = std::make_shared<rakurai::dex_connector::trading_session_interface>(
      std::move(_dex_handler->create_new_trading_session()));
}

void dex_callbacks::run()
{
   _dex_handler->run();
}

void dex_callbacks::on_pool_definition(rakurai::dex_connector::data_types::venue                          venue,
                                       const std::vector<rakurai::dex_connector::data_types::security *> &securities)
{
   SPDLOG_TRACE("On Pool definition callback");
   SPDLOG_INFO("Received {} pool definitions for venue: {}", securities.size(), static_cast<int>(venue));
   for (const auto *sec : securities)
   {
      SPDLOG_DEBUG(
         "Pool ID: {}, Venue: {}, Fee Rate BPS: {}, Min Tick: {}, Min Trade Amount: {}",
         [&]() {
            std::ostringstream oss;
            for (unsigned char each_byte : sec->pool_id)
            {
               oss << std::setw(2) << std::setfill('0') << static_cast<int>(each_byte) << " ";
            }
            return oss.str();
         }(),
         static_cast<int>(sec->venue), sec->fee_rate_bps, sec->min_tick, sec->min_trade_amount);
   }
}

void dex_callbacks::on_book(rakurai::dex_connector::data_types::venue venu, const std::array<uint8_t, 32> &pool_id,
                            const std::vector<rakurai::dex_connector::data_types::book_level> &bids,
                            const std::vector<rakurai::dex_connector::data_types::book_level> &asks)
{
   ++_update_count;
   SPDLOG_INFO("Book update no {} for Pool ID: {}", (_update_count), [&]() {
      std::ostringstream oss;
      for (unsigned char each_byte : pool_id)
      {
         oss << std::setw(2) << std::setfill('0') << static_cast<int>(each_byte) << " ";
      }
      return oss.str();
   }());

   uint count = 0;
   SPDLOG_DEBUG("Bid Book:");
   SPDLOG_DEBUG("#,Price,Size");

   for (auto each_value : bids)
   {
      SPDLOG_DEBUG("#{},{},{}", count, each_value.price, each_value.quantity);
      ++count;
   }

   count = 0;
   SPDLOG_DEBUG("Ask Book:");
   SPDLOG_DEBUG("#,Price,Size");
   for (auto each_value : asks)
   {
      SPDLOG_DEBUG("#{},{},{}", count, each_value.price, each_value.quantity);
      ++count;
   }

   if (_update_count % 5 == 0)  // Send an order every 5th book update
   {
      SPDLOG_INFO("Placing new order");
      _new_ord.order_def.pool_id    = pool_id;
      _new_ord.order_def.order_side = rakurai::dex_connector::order_entry::side::sell;
      _new_ord.order_def.dex        = rakurai::dex_connector::data_types::venue::orca_clmm;
      _new_ord.qty                  = 1000;  // 1000 lamports
      _new_ord.priority_fee         = 1000;  // 1000 lamports

      // _trade_sess_interface->launch_new_order(_new_ord);  // Uncomment this line to place actual order
      SPDLOG_INFO("Launched new order");
   }
}

void dex_callbacks::on_order_update(rakurai::dex_connector::order_entry::order_state order_state)
{
   SPDLOG_INFO("On order update callback | Client ID: {}, Txn ID: {}, Txn status: {}", order_state.client_id,
               order_state.txn_id, order_state.tx_status);
}

void dex_callbacks::on_ata_verification_status(const std::array<uint8_t, 32> &token_id, const std::string &ata_address,
                                               rakurai::dex_connector::ata_status status)
{
   SPDLOG_INFO(
      "On ATA verification | Token ID: {}, ATA address: {}, ATA status: {}",
      [&]() {
         std::ostringstream oss;
         for (unsigned char each_byte : token_id)
         {
            oss << std::setw(2) << std::setfill('0') << static_cast<int>(each_byte) << " ";
         }
         return oss.str();
      }(),
      ata_address, static_cast<int>(status));
}