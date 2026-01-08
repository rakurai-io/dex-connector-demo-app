#ifndef DEX_LISTENER_EXAMPLE_H
#define DEX_LISTENER_EXAMPLE_H

#include "dex_handler.h"
#include "dex_listener.h"
#include "order_entry_common.h"

#include <memory>

class dex_callbacks : public rakurai::dex_connector::dex_listener
{
 public:
   void   on_pool_definition(rakurai::dex_connector::data_types::venue                          venue,
                             const std::vector<rakurai::dex_connector::data_types::security *> &securities) override;
   void   on_order_update(rakurai::dex_connector::order_entry::order_state order_state) override;
   void   on_book(rakurai::dex_connector::data_types::venue venu, const std::array<uint8_t, 32> &pool_id,
                  const std::vector<rakurai::dex_connector::data_types::book_level> &bids,
                  const std::vector<rakurai::dex_connector::data_types::book_level> &asks) override;
   void   on_ata_verification_status(const std::array<uint8_t, 32> &token_id, const std::string &ata_address,
                                     rakurai::dex_connector::ata_status status) override;
   void   config();
   void   run();

 private:
   std::shared_ptr<rakurai::dex_connector::dex_handler>               _dex_handler;
   std::shared_ptr<rakurai::dex_connector::trading_session_interface> _trade_sess_interface;
   rakurai::dex_connector::order_entry::new_order                     _new_ord;
   uint                                                               _update_count = 0;
};

#endif