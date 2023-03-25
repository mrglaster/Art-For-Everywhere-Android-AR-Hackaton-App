package com.example.artforeverywhere;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;

import java.util.Objects;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Objects.requireNonNull(this.getSupportActionBar()).hide();
        setContentView(R.layout.activity_main);
        Timer timer = new Timer();
        int splashDelay = 1500;
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                Intent changeActivityIntent = new Intent(MainActivity.this, CameraActivity.class);
                startActivity(changeActivityIntent);
            }
        }, splashDelay);
    }
}