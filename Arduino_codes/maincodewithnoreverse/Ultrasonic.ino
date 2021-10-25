void ultra_sonic(){
  // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH); 
  
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    if (counter < 9) {
      distance_array[counter] = distance;
      counter +=1;
      // Displays the distance on the Serial Monitor
      return distance;
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
    } else {
        for (int i = 0; i <= 8; i++){
          distance_array[i] = distance_array[i+1];
        }
        distance_array[9] = distance;
        distance_average = 0;
        for (int i = 0; i <=9; i++){
          distance_average += distance_array[i];
        }
        distance_average = distance_average/10;
        return(distance_average);
        
        // Displays the distance on the Serial Monitor
        Serial.print("Distance: ");
        Serial.print(distance_average);
        Serial.println(" cm");
    }
}
