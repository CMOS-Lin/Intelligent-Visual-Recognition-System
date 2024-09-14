package com.example.myapplication;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class MainActivity extends AppCompatActivity {

    private MqttClient client;
    private MqttConnectOptions options;
    private Handler handler;
    private ScheduledExecutorService scheduler;

    private String productKey = "k1gbsFZaJJy";
    private String deviceName = "STM32H753ZI";
    private String deviceSecret = "55f46bf30cd2c80a44640e4c2b1482fd";

    private final String pub_topic = "/sys/k1gbsFZaJJy/STM32H753ZI/thing/event/property/post";
    private final String sub_topic = "/sys/k1gbsFZaJJy/STM32H753ZI/thing/service/property/set";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tv_S = findViewById(R.id.tv_S);
        TextView tv_C = findViewById(R.id.tv_C);
        TextView tv_T = findViewById(R.id.tv_T);
        TextView tv_R = findViewById(R.id.tv_R);
        TextView tv_B = findViewById(R.id.tv_B);
        TextView tv_G = findViewById(R.id.tv_G);

        tv_S.setText("0");
        tv_C.setText("0");
        tv_T.setText("0");
        tv_R.setText("0");
        tv_B.setText("0");
        tv_G.setText("0");

        Button btn_open = findViewById(R.id.btn_open);
        Button btn_close = findViewById(R.id.btn_close);

        btn_open.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //开
                publish_message("{\"params\":{\"LED\":1},\"version\":\"1.0.0\"}");
            }
        });

        btn_close.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //关
                publish_message("{\"params\":{\"LED\":0},\"version\":\"1.0.0\"}");
            }
        });

        mqtt_init();
        start_reconnect();

        handler = new Handler() {
            @SuppressLint("SetTextI18n")
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1: //开机校验更新回传
                        break;
                    case 2: // 反馈回传
                        break;
                    case 3: //MQTT 收到消息回传
                        String message = msg.obj.toString();
                        Log.d("nicecode", "handleMessage: " + message);
                        try {
                            JSONObject jsonObjectALL = new JSONObject(message);
                            JSONObject items = jsonObjectALL.getJSONObject("items");

                            if (items.has("S")) {
                                JSONObject obj_S = items.getJSONObject("S");
                                int value_S = obj_S.getInt("value");
                                tv_S.setText(value_S + "");
                                Log.d("nicecode", "S: " + value_S);
                            }

                            if (items.has("C")) {
                                JSONObject obj_C = items.getJSONObject("C");
                                int value_C = obj_C.getInt("value");
                                tv_C.setText(value_C + "");
                                Log.d("nicecode", "C: " + value_C);
                            }

                            if (items.has("T")) {
                                JSONObject obj_T = items.getJSONObject("T");
                                int value_T = obj_T.getInt("value");
                                tv_T.setText(value_T + "");
                                Log.d("nicecode", "T: " + value_T);
                            }

                            if (items.has("R")) {
                                JSONObject obj_R = items.getJSONObject("R");
                                int value_R = obj_R.getInt("value");
                                tv_R.setText(value_R + "");
                                Log.d("nicecode", "R: " + value_R);
                            }

                            if (items.has("B")) {
                                JSONObject obj_B = items.getJSONObject("B");
                                int value_B = obj_B.getInt("value");
                                tv_B.setText(value_B + "");
                                Log.d("nicecode", "B: " + value_B);
                            }

                            if (items.has("G")) {
                                JSONObject obj_G = items.getJSONObject("G");
                                int value_G = obj_G.getInt("value");
                                tv_G.setText(value_G + "");
                                Log.d("nicecode", "G: " + value_G);
                            }
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }
                        break;
                    case 30: //连接失败
                        Toast.makeText(MainActivity.this, "连接失败", Toast.LENGTH_SHORT).show();
                        break;
                    case 31: //连接成功
                        Toast.makeText(MainActivity.this, "连接成功", Toast.LENGTH_SHORT).show();
                        try {
                            client.subscribe(sub_topic, 1);
                        } catch (MqttException e) {
                            e.printStackTrace();
                        }
                        break;
                    default:
                        break;
                }
            }
        };

    }

    private void mqtt_init() {
        try {
            String clientId = "a1MoTKOqkVK.test_device1";
            Map<String, String> params = new HashMap<>(16);
            params.put("productKey", productKey);
            params.put("deviceName", deviceName);
            params.put("clientId", clientId);
            String timestamp = String.valueOf(System.currentTimeMillis());
            params.put("timestamp", timestamp);
            // cn-shanghai
            String host_url = "tcp://" + productKey + ".iot-as-mqtt.cn-shanghai.aliyuncs.com:1883";
            String client_id = clientId + "|securemode=2,signmethod=hmacsha1,timestamp=" + timestamp + "|";
            String user_name = deviceName + "&" + productKey;
            String password = com.example.myapplication.ui.Aliyun.sign(params, deviceSecret, "hmacsha1");

            System.out.println(">>>" + host_url);
            System.out.println(">>>" + client_id);

            client = new MqttClient(host_url, client_id, new MemoryPersistence());
            options = new MqttConnectOptions();
            options.setCleanSession(false);
            options.setUserName(user_name);
            options.setPassword(password.toCharArray());
            options.setConnectionTimeout(10);
            options.setKeepAliveInterval(60);

            client.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                    System.out.println("connectionLost----------");
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    System.out.println("deliveryComplete---------" + token.isComplete());
                }

                @Override
                public void messageArrived(String topicName, MqttMessage message) {
                    System.out.println("messageArrived----------");
                    Message msg = new Message();
                    msg.what = 3;   //收到消息标志位
                    msg.obj = new String(message.getPayload());
                    handler.sendMessage(msg);    // hander 回传
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void mqtt_connect() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    if (!client.isConnected()) {  //如果还未连接
                        client.connect(options);
                        Message msg = new Message();
                        msg.what = 31;
                        handler.sendMessage(msg);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    Message msg = new Message();
                    msg.what = 30;
                    handler.sendMessage(msg);
                }
            }
        }).start();
    }

    private void start_reconnect() {
        scheduler = Executors.newSingleThreadScheduledExecutor();
        scheduler.scheduleAtFixedRate(new Runnable() {
            @Override
            public void run() {
                if (!client.isConnected()) {
                    mqtt_connect();
                }
            }
        }, 0 * 1000, 10 * 1000, TimeUnit.MILLISECONDS);
    }

    private void publish_message(String payload) {
        try {
            MqttMessage message = new MqttMessage(payload.getBytes());
            client.publish(pub_topic, message);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}
