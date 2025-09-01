<img width="300" height="512" alt="DigitAIR" src="https://github.com/user-attachments/assets/1e9a23a8-a4c2-46bd-b907-5d0fec6c5426" />

## Descrizione del progetto
Il controllo della qualità dell’aria rappresenta una delle priorità ambientali più importanti del nostro tempo. Con l’intensificarsi dell’urbanizzazione, 
dell’industrializzazione e dei cambiamenti climatici, il monitoraggio preciso e tempestivo degli inquinanti atmosferici è diventato fondamentale per la tutela della
salute pubblica e la sostenibilità ambientale.  
Per affrontare questa sfida, l’evoluzione tecnologica degli ultimi decenni ha reso possibile lo sviluppo di sistemi di monitoraggio più capillari e
in grado di fornire i dati in tempo reale. Nel contesto territoriale pugliese, il progetto **"DigitAIR"** è stato sviluppato per superare i limiti dei tradizionali 
sistemi di monitoraggio, grazie all’implementazione di tecnologie innovative come l’Internet of Things (IoT) e l’Intelligenza Artificiale (AI).
Il progetto è stato concepito con un’architettura che consente la realizzazione di una rete di monitoraggio ad alta densità, offrendo una soluzione integrata, 
intuitiva, economicamente sostenibile e capace di garantire la massima trasparenza dei dati.  
Un aspetto fondamentale della soluzione proposta riguarda lo sviluppo di un sistema di visualizzazione dei dati accessibile a tutti. La piattaforma, caratterizzata da
un’interfaccia intuitiva e user-friendly, offre strumenti di visualizzazione intuitivi, mappe interattive per ogni inquinante e funzionalità di analisi storica
dei dati, rendendo le informazioni ambientali comprensibili e utilizzabili anche da utenti non specializzati. Particolare attenzione è stata dedicata al settore
accademico e alla pubblica amministrazione: per gli studenti universitari, i ricercatori e gli enti pubblici non solo è garantito l’accesso gratuito dei dati
raccolti consultando le mappe, ma anche l’utilizzo di funzionalità avanzate di simulazione. Queste includono modelli predittivi per valutare l’impatto 
attuale e futuro dell’inquinamento sulla salute e strumenti per stimare la riduzione degli inquinanti in seguito ad interventi di forestazione urbana.

---

## Architettura del sistema
Il sistema è progettato con un approccio modulare per garantire robustezza,scalabilità e manutenibilità, suddividendo le responsabilità tra Edge Device,
Front-end Web e Back-end, il quale è stato sviluppato secondo un’architettura a microservizi per favorire indipendenza dei componenti.

<img width="2083" height="915" alt="Architettura" src="https://github.com/user-attachments/assets/30c6c601-d99f-47ed-bbdf-bec2b0797d4c" />

Le principali componenti in cui si articola in sistema sono:  
#### User Service *([Vedi repository](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-UserService-AndreaAndrenucci-MartinaDeGaetanis))*
Il microservizio User Service gestisce gli utenti del sistema, occupandosi di registrazione, autenticazione, gestione degli account e dei domini affiliati 
da parte dell’Admin. Utilizza token JWT per garantire un accesso sicuro alle risorse e interagisce con il Notification Service per l’invio di email.

#### Notification *([Vedi repository](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-Notification-AndreaAndrenucci-MartinaDeGaetanis))*
Il microservizio gestisce l’invio delle email e comunica con il microservizio UserService tramite il protocollo AMQP, implementato con RabbitMQ.

#### Smartbox
La Smartbox è un dispositivo basato su ESP32 DevKit-C con sensori per il rilevamento degli inquinanti. Il microcontrollore, connesso alla rete, pubblica 
sul relativo topic le misure al minuto 50 di ogni ora affichè vengano salvate su un microservizio registrato al broker MQTT e gestisce eventuali disconnessioni
o malfunzionamenti tramite messaggi e API dedicate.

#### Device Indexer *([Vedi repository](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-DeviceIndexer-AndreaAndrenucci-MartinaDeGaetanis))*
Il microservizio Device Indexer mantiene il registro dei dispositivi, traccia lo stato di connessione, riceve e inoltra le misurazioni al Data Processor. 
Inoltre espone API per la gestione dei dispositivi e la loro disconnessione in caso di anomalie.

#### Data Processor *([Vedi repository](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-DeviceDataProcessor-AndreaAndrenucci-MartinaDeGaetanis))*
Il Data Processor si occupa direttamente del post processing dei dati, della generazione delle immagini e della gestione delle simulazioni. Esegue interpolazioni 
per generare mappe di distribuzione degli inquinanti e supporta simulazioni ambientali e sanitarie, fornendo inoltre API per l’accesso alle immagini e metriche.

#### Client per simulazioni *([Vedi repository](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-MqttClient-AndreaAndrenucci-MartinaDeGaetanis))*
Un client Python simula l’invio concorrente di misurazioni da più dispositivi, generando dati realistici basati sulla distribuzione geografica delle province pugliesi.

#### Gateway *([Vedi repository](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-Gateway-AndreaAndrenucci-MartinaDeGaetanis))*
I microservizi sono stati connessi al frontend utilizzando KrakenD, un gateway con configurazione basata su JSON

#### Frontend *([Vedi repository](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-Frontend-AndreaAndrenucci-MartinaDeGaetanis))*
L’interfaccia web permette agli utenti di visualizzare le mappe degli inquinanti sull’intero territorio pugliese o, a scelta, solo su Lecce. In base al ruolo, gli utenti
possono eseguire simulazioni, mentre la gestione dell’applicazione è riservata esclusivamente all’Admin.

---

Di seguito viene fornita una descrizione dettagliata della componente implementata nella repository corrente.
## Smartbox
La smartbox è realizzata utilizzando unicamente un Esp32 DevKit-C, annesso di sensoristica per la misura dei valori degli inquinanti.
Il microcontrollore, connesso alla rete, pubblica sul relativo topic le misure al minuto 50 di ogni ora. Le misure vengono salvate su un microservizio
registrato al broker. In caso di disconnessione della smartbox viene pubblicato un messaggio di last will, con id del dispositivo e info sullo stato di connessione. In caso di
problematiche dovute a un non corretto funzionamento dei sensori, l’admin può mandare una richiesta al microservizio con accesso ai dati dell’Esp32 per
richiedere la disconnessione dalla rete di quest’ultimo, via richiesta HTTP.
