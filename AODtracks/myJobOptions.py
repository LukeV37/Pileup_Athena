#jps.AthenaCommonFlags.FilesInput = [ "../../data/mc21_14TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.merge.AOD.e8481_s4149_r14701_r14702_tid33629038_00/AOD.33629038._000044.pool.root.1" ]
jps.AthenaCommonFlags.FilesInput = [ "../../data/mc21_14TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.merge.AOD.e8481_s4149_r14701_r14702/AOD.33629038._000002.pool.root.1" ]

jps.AthenaCommonFlags.AccessMode = "ClassAccess"

alg = CfgMgr.MyPackageAlg('AnalysisAlg')
athAlgSeq += alg

theApp.EvtMax = -1
jps.AthenaCommonFlags.HistOutputs = ["MYSTREAM:a1.root"]
