jps.AthenaCommonFlags.FilesInput = [ "DAOD_FTAG1.output.pool.root" ]
jps.AthenaCommonFlags.AccessMode = "ClassAccess"

alg = CfgMgr.MyPackageAlg('AnalysisAlg')
athAlgSeq += alg

theApp.EvtMax = 10
#jps.AthenaCommonFlags.HistOutputs = ["BTAGSTREAM:a1.root"]
