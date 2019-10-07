#include<stdio.h>
#include<string.h> 
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<pwd.h>
#include<grp.h>

int permissions_set() {

  char user_id[20];
  printf("\nPlease enter Username: ");
  scanf("%s", user_id);

  struct passwd * permissions_password;

  // Get user details from user input    
  if ((permissions_password = getpwnam(user_id)) != NULL) {
    printf("User ID: %d\n", permissions_password->pw_uid);
  } else {
    printf("Log in details not known");
  }

  int my_uid = permissions_password->pw_uid;
  int my_gid = permissions_password->pw_gid;
  gid_t supp_groups[] = {};
  int j;
  int ngroups;
  gid_t * groups;
  struct passwd * pw;
  struct group * gr;
  ngroups = 10;

  // Allocate memory to groups and take it back when done
  groups = malloc(ngroups * sizeof(gid_t));
  free(groups);

  if (getgrouplist(user_id, my_uid, groups, & ngroups) == -1) {
    printf("Log in successful!");
  }

  // get all the groups associated with the entered user details
  for (j = 0; j < ngroups; j++) {
    supp_groups[j] = groups[j];

  }

  uid_t uid = getuid();
  uid_t gid = getgid();
  uid_t ueid = geteuid();
  uid_t egid = getegid();

  // Change info from root to current user
  setgroups(10, supp_groups);
  setreuid(my_uid, uid);
  setregid(my_uid, gid);
  seteuid(my_uid);
  setegid(my_uid);

  return (0);
}

int permissions_reset() {

  int my_uid;
  int gid;
  int uid;

  my_uid = 0;
  setreuid(my_uid, uid);
  setregid(my_uid, gid);
  seteuid(my_uid);
  setegid(my_uid);

  return 0;
}
