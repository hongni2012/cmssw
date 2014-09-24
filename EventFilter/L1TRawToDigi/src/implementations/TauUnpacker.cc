#include "FWCore/Framework/interface/MakerMacros.h"

#include "EventFilter/L1TRawToDigi/interface/UnpackerFactory.h"

#include "L1TCollections.h"

namespace l1t {
   class TauUnpacker : public BaseUnpacker {
      public:
         using BaseUnpacker::BaseUnpacker;
         virtual bool unpack(const unsigned char *data, const unsigned block_id, const unsigned size) override;
   };

   class TauUnpackerFactory : public BaseUnpackerFactory {
      public:
         virtual std::vector<UnpackerItem> create(const unsigned& fw, const int fedid, UnpackerCollections*) override;
   };
}

// Implementation

namespace l1t {
   bool
   TauUnpacker::unpack(const unsigned char *data, const unsigned block_id, const unsigned size)
   {

     LogDebug("L1T") << "Block ID  = " << block_id << " size = " << size;

     int nBX = int(ceil(size / 8.)); // Since there are 8 Tau objects reported per event (see CMS IN-2013/005)

     // Find the first and last BXs
     int firstBX = -(ceil((double)nBX/2.)-1);
     int lastBX;
     if (nBX % 2 == 0) {
       lastBX = ceil((double)nBX/2.)+1;
     } else {
       lastBX = ceil((double)nBX/2.);
     }

     auto res_ = static_cast<L1TCollections*>(collections_)->getTaus();
     res_->setBXRange(firstBX, lastBX);

     LogDebug("L1T") << "nBX = " << nBX << " first BX = " << firstBX << " lastBX = " << lastBX;

     // Initialise index
     int unsigned i = 0;

     // Loop over multiple BX and then number of Tau cands filling collection
     for (int bx=firstBX; bx<lastBX; bx++){

       for (unsigned nTau=0; nTau < 8 && nTau < size; nTau++){

         uint32_t raw_data = pop(data,i); // pop advances the index i internally

         if (raw_data == 0)
            continue;

         l1t::Tau tau = l1t::Tau();
       
         tau.setHwPt(raw_data & 0x1FF);

	 int abs_eta = (raw_data >> 9) & 0x7F;
         if ((raw_data >> 16) & 0x1) {
           tau.setHwEta(-1*abs_eta);
         } else {
           tau.setHwEta(abs_eta);
         }

         tau.setHwPhi((raw_data >> 17) & 0xFF);
         tau.setHwIso((raw_data >> 25) & 0x1); // Assume one bit for now?
         tau.setHwQual((raw_data >> 26) & 0x7); // Assume 3 bits for now? leaves 3 spare bits

         LogDebug("L1T") << "Tau: eta " << tau.hwEta() << " phi " << tau.hwPhi() << " pT " << tau.hwPt() << " iso " << tau.hwIso() << " qual " << tau.hwQual();

         res_->push_back(bx,tau);

       }

     }

     return true;
   }

   std::vector<UnpackerItem> TauUnpackerFactory::create(const unsigned& fw, const int fedid, UnpackerCollections* coll) {

     // This unpacker is only appropriate for the Demux card output (FED ID=1). Anything else should not be unpacked.                                                     
     if (fedid==1){

       return {std::make_pair(7, std::shared_ptr<BaseUnpacker>(new TauUnpacker(coll)))};

     } else {

       return {};

     }

   };
}

DEFINE_L1TUNPACKER(l1t::TauUnpackerFactory);
