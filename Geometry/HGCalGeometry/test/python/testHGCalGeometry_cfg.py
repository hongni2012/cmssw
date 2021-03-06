import FWCore.ParameterSet.Config as cms

process = cms.Process("PROD")
process.load("SimGeneral.HepPDTESSource.pdt_cfi")

process.load("Geometry.HGCalCommonData.testHGCXML_cfi")
process.load("Geometry.HGCalCommonData.hgcalNumberingInitialization_cfi")
process.load("Geometry.HGCalCommonData.hgcalParametersInitialization_cfi")
process.load("Geometry.CaloEventSetup.HGCalTopology_cfi")
process.load("Geometry.HGCalGeometry.HGCalGeometryESProducer_cfi")

process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('cout'),
    categories = cms.untracked.vstring('HGCalGeom'),
    debugModules = cms.untracked.vstring('*'),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('DEBUG'),
        default = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        HGCalGeom = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        )
    ),
)

process.load("IOMC.RandomEngine.IOMC_cff")
process.RandomNumberGeneratorService.generator.initialSeed = 456789

process.source = cms.Source("EmptySource")

process.generator = cms.EDProducer("FlatRandomEGunProducer",
    PGunParameters = cms.PSet(
        PartID = cms.vint32(14),
        MinEta = cms.double(-3.5),
        MaxEta = cms.double(3.5),
        MinPhi = cms.double(-3.14159265359),
        MaxPhi = cms.double(3.14159265359),
        MinE   = cms.double(9.99),
        MaxE   = cms.double(10.01)
    ),
    AddAntiParticle = cms.bool(False),
    Verbosity       = cms.untracked.int32(0),
    firstRun        = cms.untracked.uint32(1)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.prodEE = cms.EDAnalyzer("HGCalGeometryTester",
                                Detector   = cms.string("HGCalEESensitive"),
                                SquareCell = cms.bool(False),
                                )

process.prodHEF = process.prodEE.clone(
    Detector   = "HGCalHESiliconSensitive",
    SquareCell = False
)

process.prodHEB = process.prodEE.clone(
    Detector   = "HGCalHEScintillatorSensitive",
    SquareCell = True
)

process.p1 = cms.Path(process.generator*process.prodEE*process.prodHEF)
