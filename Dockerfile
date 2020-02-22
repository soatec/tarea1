FROM ubuntu:18.04

#################
# Base Setup    #
#################

# Install base system software
RUN apt-get update && apt-get --yes --no-install-recommends install \
        sudo iproute2 curl git ca-certificates locales

# Set the locale
ENV LANG en_US.UTF-8
RUN locale-gen en_US.UTF-8 && update-locale LANG=en_US.UTF-8

# Install SSH Server
RUN apt-get update && apt-get --yes --no-install-recommends install \
        openssh-server && \
    mkdir -p /var/run/sshd && \
    chmod 0755 /var/run/sshd && \
    echo "StrictHostKeyChecking no" >> /etc/ssh/ssh_config && \
    sed -i 's/AcceptEnv/# AcceptEnv/' /etc/ssh/sshd_config

#################
# User Setup    #
#################

# Create and configure user
RUN echo "Creating soa user" && \
    useradd --create-home -s /bin/bash soa && \
    mkdir -p /home/soa/.ssh && \
    \
    echo "Changing soa user password" && \
    echo -n 'soa:soa' | chpasswd && \
    \
    echo "Enable passwordless sudo for the soa user" && \
    echo 'soa ALL=NOPASSWD: ALL' > /etc/sudoers.d/soa && \
    \
    echo "Configuring ssh keys" && \
    echo "Setting correct permissions to the workspace" && \
    mkdir -p /home/soa/ws && chown -R soa: /home/soa/ws

WORKDIR /tmp/
