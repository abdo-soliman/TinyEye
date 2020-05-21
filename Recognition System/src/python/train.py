import torch
import argparse
from datetime import datetime
import torchvision.transforms as transforms
from mobilefacenet import MobileFaceNet
from mobilefacenet import ArcMarginProduct
from DataLoad import read_info, DataLoad
import torch.optim as optim
import time
import numpy as np
from utils.visualize import Visualizer
from utils.logging import init_log
import os
from torch.optim import lr_scheduler


def train(args):
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

    print(device)

    save_dir = "../../models/python/"
    # logging = init_log(save_dir)
    # _print = logging.info

    # dataset loader
    transform = transforms.Compose([
        transforms.ToTensor(),  # range [0, 255] -> [0.0,1.0]
        transforms.Normalize(mean=(0.5, 0.5, 0.5), std=(
            0.5, 0.5, 0.5))  # range [0.0, 1.0] -> [-1.0,1.0]
    ])

    train_data, test_data = read_info("../../data/images.txt")

    trainset = DataLoad(train_data, transform)
    testset = DataLoad(test_data, transform)
    trainloader = torch.utils.data.DataLoader(
        trainset, batch_size=args.batch_size, shuffle=True, num_workers=8, drop_last=False)
    testloader = torch.utils.data.DataLoader(
        testset, batch_size=100, shuffle=False, num_workers=4, drop_last=False)

    # define backbone and margin layer
    net = MobileFaceNet()

    margin = ArcMarginProduct(
        args.feature_dim, trainset.class_nums)

    # define optimizers for different layer
    criterion = torch.nn.CrossEntropyLoss().to(device)
    optimizer_ft = optim.SGD([
        {'params': net.parameters(), 'weight_decay': 5e-4},
        {'params': margin.parameters(), 'weight_decay': 5e-4}
    ], lr=0.1, momentum=0.9, nesterov=True)
    exp_lr_scheduler = lr_scheduler.MultiStepLR(optimizer_ft, milestones=[6, 11, 16], gamma=0.1)

    net = net.to(device)
    margin = margin.to(device)

    best_lfw_acc = 0.0
    best_lfw_iters = 0
    best_agedb30_acc = 0.0
    best_agedb30_iters = 0
    best_cfp_fp_acc = 0.0
    best_cfp_fp_iters = 0
    total_iters = 0
    # vis = Visualizer(env="MobileFaceNet")
    for epoch in range(1, args.total_epoch + 1):
        # train model
        torch.cuda.empty_cache()
        exp_lr_scheduler.step()
        print('Train Epoch: {}/{} ...'.format(epoch, args.total_epoch))
        net.train()

        since = time.time()
        for data in trainloader:
            img, label = data[0].to(device), data[1].to(device)
            optimizer_ft.zero_grad()

            raw_logits = net(img)
            output = margin(raw_logits, label)
            total_loss = criterion(output, label)
            total_loss.backward()
            optimizer_ft.step()

            total_iters += 1
            # print train information
            if total_iters % 100 == 0:
                # current training accuracy
                _, predict = torch.max(output.data, 1)
                total = label.size(0)
                correct = (np.array(predict.cpu()) ==
                           np.array(label.data.cpu())).sum()
                time_cur = (time.time() - since) / 100
                since = time.time()
                # vis.plot_curves({'softmax loss': total_loss.item()}, iters=total_iters, title='train loss',
                #                 xlabel='iters', ylabel='train loss')
                # vis.plot_curves({'train accuracy': correct / total}, iters=total_iters, title='train accuracy', xlabel='iters',
                #                 ylabel='train accuracy')

                print("Iters: {:0>6d}/[{:0>2d}], loss: {:.4f}, train_accuracy: {:.4f}, time: {:.2f} s/iter, learning rate: {}".format(
                    total_iters, epoch, total_loss.item(), correct/total, time_cur, exp_lr_scheduler.get_lr()[0]))

            # save model
            if total_iters % args.save_freq == 0:
                msg = 'Saving checkpoint: {}'.format(total_iters)
                print(msg)
                net_state_dict = net.state_dict()
                margin_state_dict = margin.state_dict()
                if not os.path.exists(save_dir):
                    os.mkdir(save_dir)
                torch.save({
                    'iters': total_iters,
                    'net_state_dict': net_state_dict},
                    os.path.join(save_dir, 'Iter_%06d_net.ckpt' % total_iters))
                torch.save({
                    'iters': total_iters,
                    'net_state_dict': margin_state_dict},
                    os.path.join(save_dir, 'Iter_%06d_margin.ckpt' % total_iters))

            # test accuracy
        print("\n")
        net.eval()
        output = None
        for data in testloader:
            img, label = data[0].to(device), data[1].to(device)

            raw_logits = net(img)
            output = margin(raw_logits, label)
            total_loss = criterion(output, label)

        _, predict = torch.max(output.data, 1)
        total = label.size(0)
        correct = (np.array(predict.cpu()) ==
                   np.array(label.data.cpu())).sum()
        time_cur = (time.time() - since) / 100
        since = time.time()

        print("Iters: {:0>6d}/[{:0>2d}], loss: {:.4f}, test_accuracy: {:.4f}, time: {:.2f} s/iter".format(
            total_iters, epoch, total_loss.item(), correct/total, time_cur))
        print("\n")

    print('Finally Best Accuracy: LFW: {:.4f} in iters: {}, AgeDB-30: {:.4f} in iters: {} and CFP-FP: {:.4f} in iters: {}'.format(
        best_lfw_acc, best_lfw_iters, best_agedb30_acc, best_agedb30_iters, best_cfp_fp_acc, best_cfp_fp_iters))
    print('finishing training')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='PyTorch for deep face recognition')
    parser.add_argument('--train_root', type=str,
                        default='../../data/faces/', help='train image root')
    parser.add_argument('--feature_dim', type=int, default=128,
                        help='feature dimension, 128 or 512')
    parser.add_argument('--batch_size', type=int,
                        default=8, help='batch size')
    parser.add_argument('--save_freq', type=int,
                        default=3000, help='save frequency')
    parser.add_argument('--total_epoch', type=int,
                        default=16, help='total epochs')

    args = parser.parse_args()
    train(args)
