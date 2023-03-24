int client_sockets[MAX];
fd_set rfds;
//
while(1) {
	FD_ZERO(&rfds);
	FD_SET(server_socket, &rfds);
	int max_fd = server_socket;
	for (int i = 0; i < MAX; i++) {
		if (client_socket[i] > 0) 
			FD_SET(client_socket[i], &rfds);
		if (max_fd < client_socket[i]) 
			max_fd = client_socket[i];
	}
	if (select(max_fd + 1, &rfds, NULL, NULL, NULL) == -1) {
		//
	}
	if (FD_ISSET(server_socket, &rfds)) {
		if (new_socket = accept() == -1)
			//
	}
	for (int i = 0; i < MAX; i++)
		if (client_socket[i] == 0) {
			client_socket[i] = new_socket;
			break;
		}
	for (int i = 0; i < MAX; i++) {
		if (FD_ISSET(client_socket[i], &rfds)) {
			get_req(client_socket[i]);
			send_resp(client_socket[i]);
			shutdown(client_socket[i], 2);
			close(client_socket[i]);
			client_socket[i] = 0;
		}
	}
}