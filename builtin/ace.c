#define USE_THE_REPOSITORY_VARIABLE

#include "builtin.h"
#include "exec-cmd.h"
#include "run-command.h"
#include "strbuf.h"

static const char * const ace_usage[] = {
	"git ace [--ai] <subcommand> [<args>...]",
	NULL,
};

int cmd_ace(int argc, const char **argv, const char *prefix UNUSED,
	    struct repository *repo)
{
	struct child_process cp = CHILD_PROCESS_INIT;
	struct strbuf ace_path = STRBUF_INIT;
	struct strbuf ace_exec_path = STRBUF_INIT;
	struct strbuf ace_env_path = STRBUF_INIT;
	const char *old_path = getenv("PATH");
	const char *ace_dir;
	int i;

	if (!repo || !repo->worktree) {
		if (argc >= 1 && !strcmp(argv[0], "--help"))
			printf("%s\n", ace_usage[0]);
		else
			die("git ace requires a working tree");
		return 0;
	}

	ace_dir = git_executable_dir();
	if (!ace_dir)
		die("git ace requires a resolved executable directory");
	strbuf_addf(&ace_path, "%s/git-ace", ace_dir);
	strvec_push(&cp.args, ace_path.buf);
	strbuf_addf(&ace_exec_path, "GIT_EXEC_PATH=%s", ace_dir);
	strvec_push(&cp.env, ace_exec_path.buf);
	strbuf_addf(&ace_env_path, "PATH=%s%s%s", ace_dir,
		    old_path ? ":" : "", old_path ? old_path : "");
	strvec_push(&cp.env, ace_env_path.buf);
	for (i = 1; i < argc; i++)
		strvec_push(&cp.args, argv[i]);

	cp.no_stdin = 0;
	cp.stdout_to_stderr = 0;
	cp.err = STDERR_FILENO;

	if (run_command(&cp)) {
		strbuf_release(&ace_path);
		strbuf_release(&ace_exec_path);
		strbuf_release(&ace_env_path);
		return -1;
	}

	strbuf_release(&ace_path);
	strbuf_release(&ace_exec_path);
	strbuf_release(&ace_env_path);
	return 0;
}
