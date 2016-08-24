## How to run this code

```
curl https://raw.githubusercontent.com/syuvivida/DM/v0.05/scripts/signal_efficiency/untuplizer.h -o untuplizer.h

curl https://raw.githubusercontent.com/syuvivida/DM/v0.05/scripts/signal_efficiency/xAna_monoHiggs.C -o xAna_monoHiggs.C

root -q -b xAna_monoHiggs.C++\(\"xxx.root\"\)
```