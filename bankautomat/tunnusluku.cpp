#include "tunnusluku.h"
#include "ui_tunnusluku.h"
#include "valikko.h"


Tunnusluku::Tunnusluku(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tunnusluku)
{
    pinko = "";
    ui->setupUi(this);

    objectMyUrl = new MyUrl;
    base_url = objectMyUrl->getBase_url();
    tries = 3;
}

Tunnusluku::~Tunnusluku()
{
    delete ui;
    ui = nullptr;
    delete valikkoo;
    valikkoo = nullptr;
    delete objectMyUrl;
    objectMyUrl = nullptr;

}

void Tunnusluku::ResetPinWindow(){
    pinko = "";
    ui->Laatikko->setText("");
}

void Tunnusluku::on_pushButton_clicked()
{
    pinko.append("1");
    ui->Laatikko->setText(pinko); //aseta haluamasi numero laatikkoon
    emit ButtonPushed();
}

void Tunnusluku::on_pushButton_2_clicked()
{
    pinko.append("2");
    ui->Laatikko->setText(pinko);
    emit ButtonPushed();
}

void Tunnusluku::on_pushButton_3_clicked()
{
    pinko.append("3");
    ui->Laatikko->setText(pinko);
    emit ButtonPushed();
}

void Tunnusluku::on_pushButton_4_clicked()
{
    pinko.append("4");
    ui->Laatikko->setText(pinko);
    emit ButtonPushed();
}

void Tunnusluku::on_pushButton_5_clicked()
{
    pinko.append("5");
    ui->Laatikko->setText(pinko);
    emit ButtonPushed();
}
void Tunnusluku::on_pushButton_6_clicked()
{
    pinko.append("6");
    ui->Laatikko->setText(pinko);
    emit ButtonPushed();
}

void Tunnusluku::on_pushButton_7_clicked()
{
    pinko.append("7");
    ui->Laatikko->setText(pinko);
    emit ButtonPushed();
}

void Tunnusluku::on_pushButton_8_clicked()
{
    pinko.append("8");
    ui->Laatikko->setText(pinko);
    emit ButtonPushed();
}

void Tunnusluku::on_pushButton_9_clicked()
{
    pinko.append("9");
    ui->Laatikko->setText(pinko);
    emit ButtonPushed();
    if(pinko == "123459")
    {
        ui->Laatikko->setText("Avataan kortin lukitus..");
        emit Login();
        QJsonObject jsonObj;
        jsonObj.insert("cardSerial", cardSerial);
        QNetworkRequest request((base_url+"/login/unlock"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        loginManager = new QNetworkAccessManager(this);
        connect(loginManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(cardUnlockedSlot(QNetworkReply*)));
        reply = loginManager->put(request, QJsonDocument(jsonObj).toJson());
    }
}

void Tunnusluku::on_pushButton_10_clicked()
{
    pinko.append("0");
    ui->Laatikko->setText(pinko);
    emit ButtonPushed();
}

void Tunnusluku::on_pushButton_11_clicked() //voit poistaa laatikkoon tulleet numerot
{
    //ui->Laatikko->setText("poista numero");
    ui->Laatikko->setText("");
    pinko = "";
    emit ButtonPushed();
}

void Tunnusluku::on_pushButton_12_clicked() //jos oikea pinkoodi ilmestyy laatikkoon niin sulje ja näytä seuraava kohta
{                                           //jos taas pin koodi on väärä niin ilmoita yritysten määrä kunnes viimeinen on käytetty sitten sulje automaatti
    if (pinko == ""){
        ui->Laatikko->setText("Syötä pin");
        emit ButtonPushed();
    }
    else
    {
        if(tries>0){
            emit Login();
            ui->Laatikko->setText("Connecting...");
            QJsonObject jsonObj;
            jsonObj.insert("username", cardSerial);
            jsonObj.insert("password", pinko);
            QNetworkRequest request((base_url+"/login"));
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

            loginManager = new QNetworkAccessManager(this);
            connect(loginManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(loginSlot(QNetworkReply*)));

            reply = loginManager->post(request,QJsonDocument(jsonObj).toJson());
        }
        else{
            ui->Laatikko->setText("Korttisi on lukittu.");
        }
        pinko = "";
    }

    /*
      if (pinko == "1234") {

        QMessageBox msgBox;
        msgBox.setText("pinkoodi oikein");
        msgBox.exec();

        valikkoo->show();
        this-> close();
       }

    else {
        QMessageBox msgBox;
        msgBox.setText("pinkoodi väärin, yritä uudestaan");
        msgBox.exec();
      }

    */

}

void Tunnusluku::returningSlot()
{
    emit Returning();
    this->close();
}


void Tunnusluku::loginSlot(QNetworkReply *reply)
{
    response_data=reply->readAll();
    qDebug()<<"DATA : "+response_data;
    qDebug()<<"cardSerial on"+cardSerial;
    if (response_data == "false wrong password")
    {
        tries--;
        if(tries>0){
            ui->Laatikko->setText("Väärä pin, "+QString::number(tries)+" yritystä jäljellä.");
        }
        else{
            QMessageBox msgBox;
            msgBox.setText("Kortti lukittu, ota yhteyttä asiakaspalveluun");
            msgBox.exec();
            ui->Laatikko->setText("Kortti lukittu, ota yhteyttä asiakaspalveluun");
            QJsonObject jsonObj;
            jsonObj.insert("cardSerial", cardSerial);
            qAika = QDateTime::currentDateTime();
            jsonObj.insert("lockDateTime", qAika.toString("yyyy-MM-ddThh:mm:ss.zzzZ"));
            QNetworkRequest request((base_url+"/login/lock"));
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

            loginManager->deleteLater();
            loginManager = new QNetworkAccessManager(this);
            connect(loginManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(cardLockedSlot(QNetworkReply*)));
            reply = loginManager->put(request, QJsonDocument(jsonObj).toJson());
        }
    }
    else if (response_data == "card locked"){
        ui->Laatikko->setText("Korttisi on lukittu, ota yhteyttä asiakaspalveluun");
    }
    else{
        tries = 3;
        ui->Laatikko->setText("Logging in..");

        token = "Bearer "+response_data;
        name = "";
        balance = "";
        events = "";
        //cardSerial = 0600064158;
        //cardSerial = "4258145576238597";

        QNetworkRequest request((base_url+"/customers/"+cardSerial));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        //WEBTOKEN AUTH
        request.setRawHeader(QByteArray("Authorization"), (token));
        //WEBTOKEN AUTH END
        loginManager->deleteLater();
        loginManager = new QNetworkAccessManager(this);
        connect(loginManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getCustomerSlot(QNetworkReply*)));

        reply = loginManager->get(request);
    }

    //QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    //QJsonArray json_array = json_doc.array();
}


void Tunnusluku::getCustomerSlot(QNetworkReply *reply)
{
    response_data=reply->readAll();
    qDebug()<<"DATA : "+response_data;
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonArray json_array = json_doc.array();
    foreach (const QJsonValue &value, json_array) {
        QJsonObject json_obj = value.toObject();
        name+=json_obj["name"].toString();
    }
    qDebug()<<"name : "+name;

    QNetworkRequest request((base_url+"/accounts/"+cardSerial));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    //WEBTOKEN AUTH
    request.setRawHeader(QByteArray("Authorization"), (token));
    //WEBTOKEN AUTH END
    loginManager->deleteLater();
    loginManager = new QNetworkAccessManager(this);
    connect(loginManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAccountSlot(QNetworkReply*)));

    reply = loginManager->get(request);


}

void Tunnusluku::getAccountSlot(QNetworkReply *reply)
{
    response_data=reply->readAll();
    qDebug()<<"DATA : "+response_data;
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonArray json_array = json_doc.array();
    foreach (const QJsonValue &value, json_array) {
        QJsonObject json_obj = value.toObject();
        if (json_obj["balance"].toString()!=""){
        balance+=json_obj["balance"].toString();
        }
        if (json_obj["creditBalance"].toString()!=""){
            creditBalance+=json_obj["creditBalance"].toString();
        }
    }
    qDebug()<<"balance : "+balance;
    qDebug()<<"creditBalance : "+creditBalance;

    if (balance!="" && creditBalance!=""){
        QStringList items;
        items << tr("Debit") << tr("Credit");
        bool ok;
        cardType = QInputDialog::getItem( this, tr("Debit vai credit"), tr("Valitse:"), items, 0, false, &ok);
        if(ok && !cardType.isEmpty()) {}
        else{
            emit Returning();
            this->close();
        }
    }
    else if (balance!="" && creditBalance==""){
        cardType="Debit";
    }
    else if (balance=="" && creditBalance!=""){
        cardType="Credit";
    }

    if (cardType == "Debit"){
        QNetworkRequest request((base_url+"/events/"+cardSerial+"/last10"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        //WEBTOKEN AUTH
        request.setRawHeader(QByteArray("Authorization"), (token));
        //WEBTOKEN AUTH END
        loginManager->deleteLater();
        loginManager = new QNetworkAccessManager(this);
        connect(loginManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getEventsSlot(QNetworkReply*)));

        reply = loginManager->get(request);
    }
    else if (cardType == "Credit") {
        balance = creditBalance;
        QNetworkRequest request((base_url+"/events/"+cardSerial+"/last10credit"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        //WEBTOKEN AUTH
        request.setRawHeader(QByteArray("Authorization"), (token));
        //WEBTOKEN AUTH END
        loginManager->deleteLater();
        loginManager = new QNetworkAccessManager(this);
        connect(loginManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getEventsSlot(QNetworkReply*)));

        reply = loginManager->get(request);

    }
}


void Tunnusluku::getEventsSlot(QNetworkReply *reply)
{
    response_data=reply->readAll();
    qDebug()<<"DATA : "+response_data;
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonArray json_array = json_doc.array();
    foreach (const QJsonValue &value, json_array) {
        QJsonObject json_obj = value.toObject();
        if(json_obj["eventType"].toString() == "nosto" && cardType == "Debit")
        {
            events+="-"+QString::number(json_obj["amount"].toInt())+"€, "+json_obj["eventType"].toString()+", "+json_obj["dateTime"].toString().replace("-", ".");
            events.chop(14);
            events+="\n";
        }
        else if(json_obj["eventType"].toString() == "credit" && cardType == "Credit")
        {
            events+="-"+QString::number(json_obj["amount"].toInt())+"€, "+json_obj["eventType"].toString()+", "+json_obj["dateTime"].toString().replace("-", ".");
            events.chop(14);
            events+="\n";
        }
        else if (json_obj["eventType"].toString() == "talletus" && cardType == "Debit")
        {
            events+="+"+QString::number(json_obj["amount"].toInt())+"€, "+json_obj["eventType"].toString()+", "+json_obj["dateTime"].toString().replace("-", ".");
            events.chop(14);
            events+="\n";
        }

    }
    qDebug()<<"events : "+events;

    valikkoo = new valikko(name, balance, events, cardSerial, token, cardType);
    connect(valikkoo, SIGNAL(Returning()), this, SLOT(returningSlot()));
    valikkoo->show();
    this-> close();
}

void Tunnusluku::cardLockedSlot(QNetworkReply *reply)
{
    response_data=reply->readAll();
    qDebug()<<"DATA : "+response_data;
    tries = 3;
    emit Returning();
    this->close();
}

void Tunnusluku::cardUnlockedSlot(QNetworkReply *reply)
{
    response_data=reply->readAll();
    qDebug()<<"DATA : "+response_data;
    ui->Laatikko->setText("Kortti avattu");
    pinko = "";
}
