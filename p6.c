#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#define QUEUE_NAME "/my_queue"
#define MAX_MSG_SIZE 256
void sender_process() {
 mqd_t mqd;
 char msg[MAX_MSG_SIZE];
 // Open the message queue for writing
 mqd = mq_open(QUEUE_NAME, O_WRONLY);
 if (mqd == (mqd_t)-1) {
 perror("mq_open");
 exit(EXIT_FAILURE);
 }
 // Sender process
 printf("Sender process: Enter a message to send to receiver (type 'exit' to quit):\n");
 while (1) {
 fgets(msg, MAX_MSG_SIZE, stdin);
 // Send the message to the message queue
 if (mq_send(mqd, msg, strlen(msg), 0) == -1) {
 perror("mq_send");
 exit(EXIT_FAILURE);
 }
 if (strncmp(msg, "exit", 4) == 0) {
 break;
 }
 }
 // Close the message queue
 mq_close(mqd);
}
void receiver_process() {
 mqd_t mqd;
 char msg[MAX_MSG_SIZE];
 ssize_t bytes_read;
 // Open the message queue for reading
 mqd = mq_open(QUEUE_NAME, O_RDONLY);
 if (mqd == (mqd_t)-1) {
 perror("mq_open");
 exit(EXIT_FAILURE);
 }
 // Receiver process
 printf("Receiver process: Waiting for messages...\n");
 while (1) {
 // Receive a message from the message queue
 bytes_read = mq_receive(mqd, msg, MAX_MSG_SIZE, NULL);
 if (bytes_read == -1) {
 perror("mq_receive");
 exit(EXIT_FAILURE);
 }
 // Null-terminate the received message
 msg[bytes_read] = '\0';
 // Print the received message
 printf("Received message: %s", msg);
 // Check if received message is "exit" to quit the receiver process
 if (strncmp(msg, "exit", 4) == 0) {
 break;
 }
 }
 // Close the message queue
 mq_close(mqd);
}
int main() {
 // Create message queue attributes
 struct mq_attr attr;
 attr.mq_flags = 0;
 attr.mq_maxmsg = 10; // Maximum number of messages
 attr.mq_msgsize = MAX_MSG_SIZE; // Maximum message size
 attr.mq_curmsgs = 0; // Number of messages currently in queue
 // Create the message queue
 mqd_t mqd = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
 if (mqd == (mqd_t)-1) {
perror("mq_open");
 exit(EXIT_FAILURE);
 }
 // Fork a child process
 pid_t pid = fork();
 if (pid == -1) {
 perror("fork");
 exit(EXIT_FAILURE);
 } else if (pid == 0) { // Child process (receiver)
 receiver_process();
 } else { // Parent process (sender)
 sender_process();
 }
 // Unlink the message queue
 mq_unlink(QUEUE_NAME);
 return 0;
}
