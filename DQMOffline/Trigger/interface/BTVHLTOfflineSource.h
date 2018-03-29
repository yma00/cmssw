#ifndef BTVHLTOfflineSource_H
#define BTVHLTOfflineSource_H
/*
  BTVHLTOffline DQM code
*/
//
// Originally created by:  Anne-Catherine Le Bihan
//                         June 2015
// Following the structure used in JetMetHLTOfflineSource


// system include files
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <unistd.h>

// user include files
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

class BTVHLTOfflineSource : public DQMEDAnalyzer {
 public:
  explicit BTVHLTOfflineSource(const edm::ParameterSet&);
  ~BTVHLTOfflineSource() override;

 private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void bookHistograms(DQMStore::IBooker &, edm::Run const & run, edm::EventSetup const & c) override;
  void dqmBeginRun(edm::Run const& run, edm::EventSetup const& c) override;

  bool verbose_;
  std::string dirname_;
  std::string processname_;
  std::string pathname_;
  std::string filtername_;

  std::vector<std::pair<std::string, std::string> > custompathnamepairs_;

  edm::InputTag triggerSummaryLabel_;
  edm::InputTag triggerResultsLabel_;

  edm::EDGetTokenT<reco::JetTagCollection> offlineCSVTokenPF_;
  edm::EDGetTokenT<reco::JetTagCollection> offlineCSVTokenCalo_;

  edm::EDGetTokenT<std::vector<reco::Vertex> > hltFastPVToken_;
  edm::EDGetTokenT<std::vector<reco::Vertex> > hltPFPVToken_;
  edm::EDGetTokenT<std::vector<reco::Vertex> > hltCaloPVToken_;
  edm::EDGetTokenT<std::vector<reco::Vertex> > offlinePVToken_;

  edm::EDGetTokenT <edm::TriggerResults> triggerResultsToken;
  edm::EDGetTokenT <edm::TriggerResults> triggerResultsFUToken;
  edm::EDGetTokenT <trigger::TriggerEvent> triggerSummaryToken;
  edm::EDGetTokenT <trigger::TriggerEvent> triggerSummaryFUToken;

  edm::EDGetTokenT<std::vector<reco::TemplatedSecondaryVertexTagInfo<reco::IPTagInfo<edm::RefVector<std::vector<reco::Track>,reco::Track,edm::refhelper::FindUsingAdvance<std::vector<reco::Track>,reco::Track> >,reco::JTATagInfo>,reco::Vertex> > >
       csvPfTagInfosToken_;

  edm::Handle<std::vector<reco::TemplatedSecondaryVertexTagInfo<reco::IPTagInfo<edm::RefVector<std::vector<reco::Track>,reco::Track,edm::refhelper::FindUsingAdvance<std::vector<reco::Track>,reco::Track> >,reco::JTATagInfo>,reco::Vertex> > >
       csvPfTagInfos;

  edm::EDGetTokenT<reco::JetTagCollection> csvCaloTagsToken_;
  edm::EDGetTokenT<reco::JetTagCollection> csvPfTagsToken_;
  edm::Handle<reco::JetTagCollection> csvCaloTags;
  edm::Handle<reco::JetTagCollection> csvPfTags;

  HLTConfigProvider hltConfig_;
  edm::Handle<edm::TriggerResults> triggerResults_;
  edm::TriggerNames triggerNames_;
  edm::Handle<trigger::TriggerEvent> triggerObj_;

  class PathInfo {
    PathInfo():
      prescaleUsed_(-1),
      pathName_("unset"),
      filterName_("unset"),
      processName_("unset"),
      objectType_(-1),
      triggerType_("unset")
      {};

  public:
    void setHistos(
       MonitorElement* const CSV, MonitorElement* const Pt, MonitorElement* const Eta,
       MonitorElement* const CSV_RECOvsHLT, MonitorElement* const PVz, MonitorElement* const fastPVz,
       MonitorElement* const PVz_HLTMinusRECO, MonitorElement* const fastPVz_HLTMinusRECO,
       MonitorElement* const n_vtx, MonitorElement* const vtx_mass, MonitorElement* const n_vtx_trks,
       MonitorElement* const n_sel_tracks, MonitorElement* const h_3d_ip_distance,
       MonitorElement* const h_3d_ip_error, MonitorElement* const h_3d_ip_sig,
       MonitorElement* const n_pixel_hits, MonitorElement* const n_total_hits
    )
    { CSV_ = CSV; Pt_ = Pt; Eta_ = Eta; CSV_RECOvsHLT_ = CSV_RECOvsHLT; PVz_ = PVz; fastPVz_ = fastPVz;
      PVz_HLTMinusRECO_ = PVz_HLTMinusRECO; fastPVz_HLTMinusRECO_ = fastPVz_HLTMinusRECO;
      n_vtx_ = n_vtx; vtx_mass_ = vtx_mass; n_vtx_trks_ = n_vtx_trks;
      n_sel_tracks_ = n_sel_tracks; h_3d_ip_distance_ = h_3d_ip_distance;
      h_3d_ip_error_ = h_3d_ip_error; h_3d_ip_sig_ = h_3d_ip_sig;
      n_pixel_hits_ = n_pixel_hits; n_total_hits_ = n_total_hits;
    };


    ~PathInfo() = default;;
    PathInfo(int prescaleUsed,
        std::string pathName,
        std::string filterName,
        std::string processName,
        size_t type,
        std::string triggerType):
      prescaleUsed_(prescaleUsed),
      pathName_(std::move(pathName)),
      filterName_(std::move(filterName)),
      processName_(std::move(processName)),
      objectType_(type),
      triggerType_(std::move(triggerType))
    {
    }

      MonitorElement * getMEhisto_CSV()               { return CSV_;}
      MonitorElement * getMEhisto_Pt()                { return Pt_; }
      MonitorElement * getMEhisto_Eta()               { return Eta_;}
      MonitorElement * getMEhisto_CSV_RECOvsHLT()     { return CSV_RECOvsHLT_;}
      MonitorElement * getMEhisto_PVz()               { return PVz_;}
      MonitorElement * getMEhisto_fastPVz()           { return fastPVz_;}
      MonitorElement * getMEhisto_PVz_HLTMinusRECO()      { return PVz_HLTMinusRECO_;}
      MonitorElement * getMEhisto_fastPVz_HLTMinusRECO()  { return fastPVz_HLTMinusRECO_;}
      MonitorElement * getMEhisto_n_vtx()             { return n_vtx_; }
      MonitorElement * getMEhisto_vtx_mass()          { return vtx_mass_; }
      MonitorElement * getMEhisto_n_vtx_trks()        { return n_vtx_trks_; }
      MonitorElement * getMEhisto_n_sel_tracks()      { return n_sel_tracks_; }
      MonitorElement * getMEhisto_h_3d_ip_distance()  { return h_3d_ip_distance_; }
      MonitorElement * getMEhisto_h_3d_ip_error()     { return h_3d_ip_error_; }
      MonitorElement * getMEhisto_h_3d_ip_sig()       { return h_3d_ip_sig_; }
      MonitorElement * getMEhisto_n_pixel_hits()      { return n_pixel_hits_; }
      MonitorElement * getMEhisto_n_total_hits()            { return n_total_hits_; }

      const std::string getLabel( ) const {
          return filterName_;
      }
      void setLabel(std::string labelName){
          filterName_ = std::move(labelName);
          return;
      }
      const std::string getPath( ) const {
          return pathName_;
      }
      const int getprescaleUsed() const {
          return prescaleUsed_;
      }
      const std::string getProcess( ) const {
          return processName_;
      }
      const int getObjectType( ) const {
          return objectType_;
      }
      const std::string getTriggerType( ) const {
          return triggerType_;
      }
      const edm::InputTag getTag() const{
          edm::InputTag tagName(filterName_,"",processName_);
          return tagName;
      }
      bool operator==(const std::string& v)
      {
          return v==pathName_;
      }

  private:

      int prescaleUsed_;
      std::string pathName_;
      std::string filterName_;
      std::string processName_;
      int objectType_;
      std::string triggerType_;

      MonitorElement*  CSV_;
      MonitorElement*  Pt_;
      MonitorElement*  Eta_;
      MonitorElement*  CSV_RECOvsHLT_;
      MonitorElement*  PVz_;
      MonitorElement*  fastPVz_;
      MonitorElement*  PVz_HLTMinusRECO_;
      MonitorElement*  fastPVz_HLTMinusRECO_;
      MonitorElement*  n_vtx_;
      MonitorElement*  vtx_mass_;
      MonitorElement*  n_vtx_trks_;
      MonitorElement*  n_sel_tracks_;
      MonitorElement*  h_3d_ip_distance_;
      MonitorElement*  h_3d_ip_error_;
      MonitorElement*  h_3d_ip_sig_;
      MonitorElement*  n_pixel_hits_;
      MonitorElement*  n_total_hits_;


      };

  class PathInfoCollection: public std::vector<PathInfo> {
  public:
    PathInfoCollection(): std::vector<PathInfo>()
      {};
      std::vector<PathInfo>::iterator find(const std::string& pathName) {
        return std::find(begin(), end(), pathName);
      }
  };
  PathInfoCollection hltPathsAll_;

 };
#endif
