# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.require_version ">= 1.8.1"
ENV['VAGRANT_DEFAULT_PROVIDER'] = 'docker'

if !Vagrant.has_plugin?("vagrant-proxyconf")
    system('vagrant plugin install vagrant-proxyconf')
    raise("vagrant-proxyconf installed. Run command again.")
end

if !ENV.has_key?('SSH_AUTH_SOCK')
    raise("SSH_AUTH_SOCK not found, please setup your ssh-agent")
end

image_name = "5f59b05083cc"

Vagrant.configure("2") do |config|
  config.vm.define "soa", primary: true do |soa|
    soa.vm.hostname = "tarea1-soa"
    soa.vm.provider "docker" do |docker|
      docker.image = image_name
      docker.has_ssh = true
      docker.pull = false
      docker.create_args = %w(--privileged)
    end
    soa.vm.usable_port_range = 2200..2299
    soa.ssh.username = "soa"
    soa.ssh.forward_agent = true
    soa.ssh.pty = true
    soa.ssh.forward_x11 = true
    if Vagrant.has_plugin?("vagrant-proxyconf")
      soa.proxy.http = ENV["http_proxy"]
      soa.proxy.https = ENV["https_proxy"]
      soa.proxy.no_proxy = ENV["no_proxy"]
    end
  end
end
