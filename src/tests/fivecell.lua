local function make_thin_fivecell()
  local sequ = require 'sequence'
  local elem = require 'element'
  
  local multipole = elem.multipole
  local kicker    = elem.kicker
  local monitor   = elem.monitor
  local marker    = elem.marker
  
  local bang =  0.005099988074
  local kqf  =  0.00872651312
  local kqd  = -0.00872777242
  local ksf  =  0.0198492943 / 2  -- (n-1)!
  local ksd  = -0.0396212830 / 2  -- (n-1)!
  
  local mb    = multipole 'mb'   { lrad = 14.2 * math.sin ( bang * 0.5 ) / ( bang * 0.5 ), knl = { bang } }
  local qf    = multipole 'qf'   { lrad = 0.62, knl = { 0, kqf * 3.1 * 0.2 } }
  local qd    = multipole 'qd'   { lrad = 0.62, knl = { 0, kqd * 3.1 * 0.2 } }
  local sf    = multipole 'sf'   { lrad = 1.1 , knl = { 0, 0, ksf * 1.1 } }
  local sd    = multipole 'sd'   { lrad = 1.1 , knl = { 0, 0, ksd * 1.1 } }
  local kh    = kicker    'cbh'  { hkick = 0 }
  local kv    = kicker    'cbv'  { vkick = 0 }
  local bph   = monitor   'bph'  {}
  local bpv   = monitor   'bpv'  {}
  local mk    = marker
  
  local fivecell = sequ 'fivecell' { length = 534.6 }
    + { qf  'qf.1..1'       {}, at = 0.2583333333 }
    + { qf  'qf.1..2'       {}, at = 0.9041666667 }
    + { mk  'qf.1'          {}, at = 1.55 }
    + { qf  'qf.1..3'       {}, at = 1.55 }
    + { qf  'qf.1..4'       {}, at = 2.195833333 }
    + { qf  'qf.1..5'       {}, at = 2.841666667 }
    + { sf  'mscbh.1'       {}, at = 3.815 }
    + { kh  'cbh.1'         {}, at = 4.365 }
    + { mb  'mb'            {}, at = 12.62 }
    + { mb  'mb'            {}, at = 28.28 }
    + { mb  'mb'            {}, at = 43.94 }
    + { bpv 'bpv.1'         {}, at = 52.46 }
    + { qd  'qd.1..1'       {}, at = 53.71833333 }
    + { qd  'qd.1..2'       {}, at = 54.36416667 }
    + { mk  'qd.1'          {}, at = 55.01 }
    + { qd  'qd.1..3'       {}, at = 55.01 }
    + { qd  'qd.1..4'       {}, at = 55.65583333 }
    + { qd  'qd.1..5'       {}, at = 56.30166667 }
    + { sd  'mscbv.1'       {}, at = 57.275 }
    + { kv  'cbv.1'         {}, at = 57.825 }
    + { mb  'mb'            {}, at = 66.08 }
    + { mb  'mb'            {}, at = 81.74 }
    + { mb  'mb'            {}, at = 97.4 }
    + { bph 'bph.1'         {}, at = 105.92 }
    
    + { qf  'qf.2..1'       {}, at = 107.1783333 }
    + { qf  'qf.2..2'       {}, at = 107.8241667 }
    + { mk  'qf.2'          {}, at = 108.47 }
    + { qf  'qf.2..3'       {}, at = 108.47 }
    + { qf  'qf.2..4'       {}, at = 109.1158333 }
    + { qf  'qf.2..5'       {}, at = 109.7616667 }
    + { sf  'mscbh.2'       {}, at = 110.735 }
    + { kh  'cbh.2'         {}, at = 111.285 }
    + { mb  'mb'            {}, at = 119.54 }
    + { mb  'mb'            {}, at = 135.2 }
    + { mb  'mb'            {}, at = 150.86 }
    + { bpv 'bpv.2'         {}, at = 159.38 }
    + { qd  'qd.2..1'       {}, at = 160.6383333 }
    + { qd  'qd.2..2'       {}, at = 161.2841667 }
    + { mk  'qd.2'          {}, at = 161.93 }
    + { qd  'qd.2..3'       {}, at = 161.93 }
    + { qd  'qd.2..4'       {}, at = 162.5758333 }
    + { qd  'qd.2..5'       {}, at = 163.2216667 }
    + { sd  'mscbv.2'       {}, at = 164.195 }
    + { kv  'cbv.2'         {}, at = 164.745 }
    + { mb  'mb'            {}, at = 173 }
    + { mb  'mb'            {}, at = 188.66 }
    + { mb  'mb'            {}, at = 204.32 }
    + { bph 'bph.2'         {}, at = 212.84 }
    
    + { qf  'qf.3..1'       {}, at = 214.0983333 }
    + { qf  'qf.3..2'       {}, at = 214.7441667 }
    + { mk  'qf.3'          {}, at = 215.39 }
    + { qf  'qf.3..3'       {}, at = 215.39 }
    + { qf  'qf.3..4'       {}, at = 216.0358333 }
    + { qf  'qf.3..5'       {}, at = 216.6816667 }
    + { sf  'mscbh.3'       {}, at = 217.655 }
    + { kh  'cbh.3'         {}, at = 218.205 }
    + { mb  'mb'            {}, at = 226.46 }
    + { mb  'mb'            {}, at = 242.12 }
    + { mb  'mb'            {}, at = 257.78 }
    + { bpv 'bpv.3'         {}, at = 266.3 }
    + { qd  'qd.3..1'       {}, at = 267.5583333 }
    + { qd  'qd.3..2'       {}, at = 268.2041667 }
    + { mk  'qd.3'          {}, at = 268.85 }
    + { qd  'qd.3..3'       {}, at = 268.85 }
    + { qd  'qd.3..4'       {}, at = 269.4958333 }
    + { qd  'qd.3..5'       {}, at = 270.1416667 }
    + { sd  'mscbv.3'       {}, at = 271.115 }
    + { kv  'cbv.3'         {}, at = 271.665 }
    + { mb  'mb'            {}, at = 279.92 }
    + { mb  'mb'            {}, at = 295.58 }
    + { mb  'mb'            {}, at = 311.24 }
    + { bph 'bph.3'         {}, at = 319.76 }
    
    + { qf  'qf.4..1'       {}, at = 321.0183333 }
    + { qf  'qf.4..2'       {}, at = 321.6641667 }
    + { mk  'qf.4'          {}, at = 322.31 }
    + { qf  'qf.4..3'       {}, at = 322.31 }
    + { qf  'qf.4..4'       {}, at = 322.9558333 }
    + { qf  'qf.4..5'       {}, at = 323.6016667 }
    + { sf  'mscbh.4'       {}, at = 324.575 }
    + { kh  'cbh.4'         {}, at = 325.125 }
    + { mb  'mb'            {}, at = 333.38 }
    + { mb  'mb'            {}, at = 349.04 }
    + { mb  'mb'            {}, at = 364.7 }
    + { bpv 'bpv.4'         {}, at = 373.22 }
    + { qd  'qd.4..1'       {}, at = 374.4783333 }
    + { qd  'qd.4..2'       {}, at = 375.1241667 }
    + { mk  'qd.4'          {}, at = 375.77 }
    + { qd  'qd.4..3'       {}, at = 375.77 }
    + { qd  'qd.4..4'       {}, at = 376.4158333 }
    + { qd  'qd.4..5'       {}, at = 377.0616667 }
    + { sd  'mscbv.4'       {}, at = 378.035 }
    + { kv  'cbv.4'         {}, at = 378.585 }
    + { mb  'mb'            {}, at = 386.84 }
    + { mb  'mb'            {}, at = 402.5 }
    + { mb  'mb'            {}, at = 418.16 }
    + { bph 'bph.4'         {}, at = 426.68 }
    
    + { qf  'qf.5..1'       {}, at = 427.9383333 }
    + { qf  'qf.5..2'       {}, at = 428.5841667 }
    + { mk  'qf.5'          {}, at = 429.23 }
    + { qf  'qf.5..3'       {}, at = 429.23 }
    + { qf  'qf.5..4'       {}, at = 429.8758333 }
    + { qf  'qf.5..5'       {}, at = 430.5216667 }
    + { sf  'mscbh.5'       {}, at = 431.495 }
    + { kh  'cbh.5'         {}, at = 432.045 }
    + { mb  'mb'            {}, at = 440.3 }
    + { mb  'mb'            {}, at = 455.96 }
    + { mb  'mb'            {}, at = 471.62 }
    + { bpv 'bpv.5'         {}, at = 480.14 }
    + { qd  'qd.5..1'       {}, at = 481.3983333 }
    + { qd  'qd.5..2'       {}, at = 482.0441667 }
    + { mk  'qd.5'          {}, at = 482.69 }
    + { qd  'qd.5..3'       {}, at = 482.69 }
    + { qd  'qd.5..4'       {}, at = 483.3358333 }
    + { qd  'qd.5..5'       {}, at = 483.9816667 }
    + { sd  'mscbv.5'       {}, at = 484.955 }
    + { kv  'cbv.5'         {}, at = 485.505 }
    + { mb  'mb'            {}, at = 493.76 }
    + { mb  'mb'            {}, at = 509.42 }
    + { mb  'mb'            {}, at = 525.08 }
    + { bph 'bph.5'         {}, at = 533.6 }

  return fivecell:done()
end

return { thin = make_thin_fivecell }