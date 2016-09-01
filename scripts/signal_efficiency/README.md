## How to run this code (boosted)

```
curl https://raw.githubusercontent.com/syuvivida/DM/v0.05/scripts/signal_efficiency/untuplizer.h -o untuplizer.h

curl https://raw.githubusercontent.com/syuvivida/DM/v0.05/scripts/signal_efficiency/boosted_xAna_monoHiggs.C -o boosted_xAna_monoHiggs.C

root -q -b boosted_xAna_monoHiggs.C++\(\"xxx.root\"\)
```


## How to run this code (resolved)

```
curl https://raw.githubusercontent.com/syuvivida/DM/v0.05/scripts/signal_efficiency/untuplizer.h -o untuplizer.h

curl https://raw.githubusercontent.com/syuvivida/DM/v0.05/scripts/signal_efficiency/resolved_xAna_monoHiggs.C -o resolved_xAna_monoHiggs.C

root -q -b resolved_xAna_monoHiggs.C++\(\"xxx.root\"\)
```